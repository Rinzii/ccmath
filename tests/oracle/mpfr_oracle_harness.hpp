#pragma once

#include "oracle_campaign_common.hpp"
#include "utils/fenv_fixture.hpp"
#include "utils/ulp_distance.hpp"

#include <mpfr.h>

#include <cfenv>
#include <cmath>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

namespace ccm::test::oracle
{
	inline mpfr_rnd_t fe_to_mpfr_rounding(int fe_mode)
	{
		switch (fe_mode)
		{
		case FE_UPWARD: return MPFR_RNDU;
		case FE_DOWNWARD: return MPFR_RNDD;
		case FE_TOWARDZERO: return MPFR_RNDZ;
		case FE_TONEAREST:
		default: return MPFR_RNDN;
		}
	}

	// The reference rounds to the MPFR default rounding mode, which the sweep keeps in
	// lockstep with the active hardware mode. Reading the default here means the oracle and
	// the function under test always agree on the mode being measured.
	inline mpfr_rnd_t current_mpfr_rounding_mode()
	{
		return mpfr_get_default_rounding_mode();
	}

	// Set the active hardware rounding mode and the MPFR default rounding mode together so a
	// directed-rounding campaign measures the function under test and the oracle in the same
	// mode. Both are restored on scope exit.
	class ScopedMpfrRoundingMode
	{
	public:
		explicit ScopedMpfrRoundingMode(int fe_mode) : guard_(fe_mode), previous_(mpfr_get_default_rounding_mode())
		{
			if (guard_) { mpfr_set_default_rounding_mode(fe_to_mpfr_rounding(fe_mode)); }
		}

		~ScopedMpfrRoundingMode() { mpfr_set_default_rounding_mode(previous_); }

		ScopedMpfrRoundingMode(const ScopedMpfrRoundingMode &)			  = delete;
		ScopedMpfrRoundingMode &operator=(const ScopedMpfrRoundingMode &) = delete;

		explicit operator bool() const { return static_cast<bool>(guard_); }

	private:
		ccm::test::ForceRoundingMode guard_;
		mpfr_rnd_t previous_;
	};

	template <typename T>
	inline void mpfr_set_scalar(mpfr_t value, T input)
	{
		if constexpr (std::is_same_v<T, float>) { mpfr_set_flt(value, input, MPFR_RNDN); }
		else if constexpr (std::is_same_v<T, double>) { mpfr_set_d(value, input, MPFR_RNDN); }
		else if constexpr (std::is_same_v<T, long double>) { mpfr_set_ld(value, input, MPFR_RNDN); }
	}

	template <typename T>
	inline T mpfr_get_scalar(const mpfr_t value, mpfr_rnd_t rounding)
	{
		if constexpr (std::is_same_v<T, float>) { return mpfr_get_flt(value, rounding); }
		else if constexpr (std::is_same_v<T, double>) { return mpfr_get_d(value, rounding); }
		else if constexpr (std::is_same_v<T, long double>) { return mpfr_get_ld(value, rounding); }
	}

	template <typename T>
	inline bool oracle_match(T actual, T expected, std::uint64_t max_ulp, std::uint64_t &ulp_distance, std::string &notes)
	{
		const auto distance = ccm::test::ulp::classify_distance(actual, expected);
		ulp_distance		= distance.distance;

		if (std::isnan(expected))
		{
			if (!std::isnan(actual))
			{
				notes = "expected NaN from reference oracle";
				return false;
			}
			return true;
		}
		if (std::isinf(expected))
		{
			if (actual != expected)
			{
				notes = "infinity mismatch";
				return false;
			}
			return true;
		}
		if (actual == T{} && expected == T{})
		{
			if (std::signbit(actual) != std::signbit(expected))
			{
				notes = "signed-zero mismatch";
				return false;
			}
			return true;
		}
		if (distance.kind != ccm::test::ulp::relation::finite)
		{
			notes = "non-finite mismatch";
			return false;
		}
		if (distance.distance > max_ulp)
		{
			notes = "ULP threshold exceeded";
			return false;
		}
		return true;
	}

	template <typename T>
	inline bool exceptional_or_zero_match(T actual, T reference)
	{
		if (std::isnan(actual)) { return std::isnan(reference); }
		if (std::isinf(actual)) { return actual == reference; }
		if (actual == T{} && reference == T{}) { return std::signbit(actual) == std::signbit(reference); }
		return actual == reference;
	}

	template <typename T>
	inline bool is_exceptional_or_zero_result(T value)
	{
		return std::isnan(value) || std::isinf(value) || value == T{};
	}

	template <typename T, typename ActualFn, typename ReferenceFn, typename ExceptionalReferenceFn, typename UsesFullOracleFn, typename SkipReasonFn>
	inline std::optional<failure_record<T>> evaluate_binary_mpfr_case(const pow_case<T> &test_case,
																	  std::string_view function_name,
																	  std::string_view path_name,
																	  ccm::test::pow_path::validation_path path,
																	  ActualFn actual_fn,
																	  ReferenceFn reference_fn,
																	  ExceptionalReferenceFn exceptional_reference_fn,
																	  UsesFullOracleFn uses_full_oracle_fn,
																	  SkipReasonFn skip_reason_fn,
																	  mpfr_prec_t oracle_precision,
																	  std::uint64_t max_ulp,
																	  run_summary<T> &summary,
																	  std::uint64_t target_ulp					= 0,
																	  std::uint64_t seed						= 0,
																	  std::string_view search_mode				= {},
																	  std::string_view mismatch_note			= "exceptional mismatch vs std reference",
																	  std::vector<failure_record<T>> *event_log = nullptr)
	{
		if (const auto skip = skip_reason_fn(path, test_case.base, test_case.exponent))
		{
			++summary.skipped_count;
			return std::nullopt;
		}

		++summary.case_count;

		const mpfr_rnd_t rounding = current_mpfr_rounding_mode();
		const T actual			  = actual_fn(test_case.base, test_case.exponent);

		const auto finish_case = [&](T expected) -> std::optional<failure_record<T>>
		{
			std::uint64_t distance = 0;
			std::string notes;
			const bool pass = oracle_match(actual, expected, max_ulp, distance, notes);
			record_worst_case(summary, distance, test_case.base, test_case.exponent, actual, expected);
			if (pass)
			{
				if (distance > target_ulp)
				{
					++summary.above_target_count;
					if (event_log != nullptr)
					{
						event_log->push_back(make_failure_record(function_name,
																 path_name,
																 test_case.provenance,
																 test_case.base,
																 test_case.exponent,
																 actual,
																 expected,
																 distance,
																 current_rounding_mode_name(),
																 static_cast<unsigned long>(oracle_precision),
																 seed,
																 search_mode,
																 "above correctly-rounded target",
																 "mpfr_above_target"));
					}
				}
				return std::nullopt;
			}

			++summary.failure_count;
			auto record = make_failure_record(function_name,
											  path_name,
											  test_case.provenance,
											  test_case.base,
											  test_case.exponent,
											  actual,
											  expected,
											  distance,
											  current_rounding_mode_name(),
											  static_cast<unsigned long>(oracle_precision),
											  seed,
											  search_mode,
											  notes,
											  "mpfr_hard_failure");
			if (event_log != nullptr) { event_log->push_back(record); }
			return record;
		};

		if (!uses_full_oracle_fn(path))
		{
			const T fallback_expected = exceptional_reference_fn(test_case.base, test_case.exponent);
			const T mpfr_expected	  = reference_fn(test_case.base, test_case.exponent, oracle_precision, rounding);

			if (is_exceptional_or_zero_result(actual) || is_exceptional_or_zero_result(fallback_expected))
			{
				// MPFR is the correctly-rounded oracle; std::pow is only the fallback for IEEE
				// special-case conventions MPFR may not model (signed zeros, inf/NaN from C rules).
				// A result that matches the correctly-rounded MPFR value is never a failure, even
				// when the platform std::pow disagrees: e.g. pow(2, -1074) is exactly the smallest
				// subnormal, which some libm builds flush to zero under directed rounding.
				const bool matches_std	= exceptional_or_zero_match(actual, fallback_expected);
				const bool matches_mpfr = exceptional_or_zero_match(actual, mpfr_expected);
				if (matches_std || matches_mpfr)
				{
					if (matches_std && !matches_mpfr) { ++summary.mpfr_policy_mismatch_count; }
					return std::nullopt;
				}

				++summary.failure_count;
				auto record = make_failure_record(function_name,
												  path_name,
												  test_case.provenance,
												  test_case.base,
												  test_case.exponent,
												  actual,
												  fallback_expected,
												  0,
												  current_rounding_mode_name(),
												  static_cast<unsigned long>(oracle_precision),
												  seed,
												  search_mode,
												  mismatch_note,
												  "mpfr_hard_failure");
				if (event_log != nullptr) { event_log->push_back(record); }
				return record;
			}

			return finish_case(mpfr_expected);
		}

		return finish_case(reference_fn(test_case.base, test_case.exponent, oracle_precision, rounding));
	}
} // namespace ccm::test::oracle
