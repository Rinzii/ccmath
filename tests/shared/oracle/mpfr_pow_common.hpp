#pragma once

#include "ccmath/internal/config/long_double_format.hpp"
#include "ccmath/internal/math/generic/builtins/power/pow.hpp"
#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"
#include "mpfr_oracle_harness.hpp"

namespace ccm::test::oracle
{
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

	template <typename T, typename Fn>
	inline std::optional<failure_record<T>> evaluate_case(const pow_case<T>& test_case,
														  std::string_view function_name,
														  std::string_view path_name,
														  ccm::test::pow_path::validation_path path,
														  Fn fn,
														  mpfr_prec_t oracle_precision,
														  std::uint64_t max_ulp,
														  run_summary<T>& summary,
														  std::uint64_t target_ulp					= 0,
														  std::uint64_t seed						= 0,
														  std::string_view search_mode				= {},
														  std::vector<failure_record<T>>* event_log = nullptr)
	{
		return evaluate_binary_mpfr_case(
			test_case,
			function_name,
			path_name,
			path,
			fn,
			[](T base, T exponent, mpfr_prec_t precision, mpfr_rnd_t rounding) { return mpfr_pow_reference(base, exponent, precision, rounding); },
			[](T base, T exponent) { return static_cast<T>(std::pow(base, exponent)); },
			[](ccm::test::pow_path::validation_path candidate_path) { return uses_public_mpfr_oracle(candidate_path); },
			[](ccm::test::pow_path::validation_path candidate_path, T base, T exponent) { return kernel_path_skip_reason(candidate_path, base, exponent); },
			oracle_precision,
			max_ulp,
			summary,
			target_ulp,
			seed,
			search_mode,
			"exceptional mismatch vs std::pow",
			event_log);
	}
} // namespace ccm::test::oracle
