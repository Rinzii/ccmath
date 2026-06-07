#pragma once

#include "oracle_campaign_common.hpp"

#include "ccmath/internal/config/long_double_format.hpp"
#include "ccmath/internal/math/generic/builtins/power/pow.hpp"
#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"
#include "utils/ulp_distance.hpp"
#include "utils/worst_case_samples.hpp"

#include <mpfr.h>

#include <cmath>
#include <optional>
#include <string>
#include <string_view>

namespace ccm::test::oracle
{
	inline mpfr_rnd_t current_mpfr_rounding_mode()
	{
		switch (std::fegetround())
		{
		case FE_UPWARD: return MPFR_RNDU;
		case FE_DOWNWARD: return MPFR_RNDD;
		case FE_TOWARDZERO: return MPFR_RNDZ;
		case FE_TONEAREST:
		default: return MPFR_RNDN;
		}
	}

	inline bool mpfr_long_double_supported()
	{
#if defined(MPFR_VERSION) && (MPFR_VERSION >= MPFR_VERSION_NUM(3, 0, 0))
		return true;
#else
		return false;
#endif
	}

	inline mpfr_prec_t powl_oracle_precision(ccm::config::LongDoubleFormat format)
	{
		switch (format)
		{
		case ccm::config::LongDoubleFormat::Double: return 256;
		case ccm::config::LongDoubleFormat::X87Extended: return 256;
		case ccm::config::LongDoubleFormat::IEEEBinary128: return 256;
		case ccm::config::LongDoubleFormat::Unknown: return 256;
		}
		return 256;
	}

	inline bool powl_exceptional_case(long double base, long double exponent)
	{
		if (std::isnan(base) || std::isnan(exponent)) { return true; }
		if (!std::isfinite(base) || !std::isfinite(exponent)) { return true; }
		if (base == 0.0L) { return true; }
		if (base < 0.0L) { return true; }
		if (exponent == 0.0L) { return true; }
		if (base == 1.0L) { return true; }
		return false;
	}

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
	inline T mpfr_pow_reference(T base, T exponent, mpfr_prec_t precision, mpfr_rnd_t rounding)
	{
		mpfr_t base_mpfr;
		mpfr_t exp_mpfr;
		mpfr_t result_mpfr;
		mpfr_init2(base_mpfr, precision);
		mpfr_init2(exp_mpfr, precision);
		mpfr_init2(result_mpfr, precision);

		mpfr_set_scalar(base_mpfr, base);
		mpfr_set_scalar(exp_mpfr, exponent);
		mpfr_pow(result_mpfr, base_mpfr, exp_mpfr, rounding);

		const T result = mpfr_get_scalar<T>(result_mpfr, rounding);
		mpfr_clear(base_mpfr);
		mpfr_clear(exp_mpfr);
		mpfr_clear(result_mpfr);
		return result;
	}

	template <typename T>
	inline bool oracle_match(T actual, T expected, std::uint64_t max_ulp, std::uint64_t & ulp_distance, std::string & notes)
	{
		const auto distance = ccm::test::ulp::classify_distance(actual, expected);
		ulp_distance		= distance.distance;

		if (std::isnan(expected))
		{
			if (!std::isnan(actual))
			{
				notes = "expected NaN from MPFR";
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

	template <typename T, typename Fn>
	inline std::optional<failure_record<T>> evaluate_case(const pow_case<T> & test_case,
														  std::string_view function_name,
														  std::string_view path_name,
														  ccm::test::pow_path::validation_path path,
														  Fn fn,
														  mpfr_prec_t oracle_precision,
														  std::uint64_t max_ulp,
														  run_summary<T> & summary,
														  std::uint64_t seed		   = 0,
														  std::string_view search_mode = {})
	{
		if (const auto skip = kernel_path_skip_reason(path, test_case.base, test_case.exponent))
		{
			++summary.skipped_count;
			return std::nullopt;
		}

		++summary.case_count;

		const mpfr_rnd_t rounding = current_mpfr_rounding_mode();
		const T actual			  = fn(test_case.base, test_case.exponent);

		if (!uses_public_mpfr_oracle(path))
		{
			const T std_ref	 = std::pow(test_case.base, test_case.exponent);
			const T mpfr_ref = mpfr_pow_reference(test_case.base, test_case.exponent, oracle_precision, rounding);

			if (is_exceptional_or_zero_result(actual) || is_exceptional_or_zero_result(std_ref))
			{
				if (exceptional_or_zero_match(actual, std_ref))
				{
					if (!exceptional_or_zero_match(actual, mpfr_ref)) { ++summary.mpfr_policy_mismatch_count; }
					return std::nullopt;
				}

				++summary.failure_count;
				return failure_record<T>{
					std::string(function_name),
					scalar_type_name<T>(),
					std::string(path_name),
					test_case.provenance,
					test_case.base,
					test_case.exponent,
					actual,
					std_ref,
					bits_hex(test_case.base),
					bits_hex(test_case.exponent),
					bits_hex(actual),
					bits_hex(std_ref),
					0,
					current_rounding_mode_name(),
					static_cast<unsigned long>(oracle_precision),
					compiler_id(),
					platform_id(),
					fma_status(),
					builtin_status<T>(),
					simd_status(),
					configuration_name(),
					optimization_mode(),
					std::string(search_mode),
					seed,
					utc_timestamp(),
					"exceptional mismatch vs std::pow",
				};
			}

			std::uint64_t distance = 0;
			std::string notes;
			const bool pass = oracle_match(actual, mpfr_ref, max_ulp, distance, notes);
			if (distance >= summary.max_observed_ulp)
			{
				summary.max_observed_ulp = distance;
				summary.worst_base		 = test_case.base;
				summary.worst_exponent	 = test_case.exponent;
				summary.worst_actual	 = actual;
				summary.worst_expected	 = mpfr_ref;
			}
			if (pass) { return std::nullopt; }

			++summary.failure_count;
			return failure_record<T>{
				std::string(function_name),
				scalar_type_name<T>(),
				std::string(path_name),
				test_case.provenance,
				test_case.base,
				test_case.exponent,
				actual,
				mpfr_ref,
				bits_hex(test_case.base),
				bits_hex(test_case.exponent),
				bits_hex(actual),
				bits_hex(mpfr_ref),
				distance,
				current_rounding_mode_name(),
				static_cast<unsigned long>(oracle_precision),
				compiler_id(),
				platform_id(),
				fma_status(),
				builtin_status<T>(),
				simd_status(),
				configuration_name(),
				optimization_mode(),
				std::string(search_mode),
				seed,
				utc_timestamp(),
				notes,
			};
		}

		const T expected = mpfr_pow_reference(test_case.base, test_case.exponent, oracle_precision, rounding);

		std::uint64_t distance = 0;
		std::string notes;
		const bool pass = oracle_match(actual, expected, max_ulp, distance, notes);
		if (distance >= summary.max_observed_ulp)
		{
			summary.max_observed_ulp = distance;
			summary.worst_base		 = test_case.base;
			summary.worst_exponent	 = test_case.exponent;
			summary.worst_actual	 = actual;
			summary.worst_expected	 = expected;
		}
		if (pass) { return std::nullopt; }

		++summary.failure_count;
		return failure_record<T>{
			std::string(function_name),
			scalar_type_name<T>(),
			std::string(path_name),
			test_case.provenance,
			test_case.base,
			test_case.exponent,
			actual,
			expected,
			bits_hex(test_case.base),
			bits_hex(test_case.exponent),
			bits_hex(actual),
			bits_hex(expected),
			distance,
			current_rounding_mode_name(),
			static_cast<unsigned long>(oracle_precision),
			compiler_id(),
			platform_id(),
			fma_status(),
			builtin_status<T>(),
			simd_status(),
			configuration_name(),
			optimization_mode(),
			std::string(search_mode),
			seed,
			utc_timestamp(),
			notes,
		};
	}
} // namespace ccm::test::oracle
