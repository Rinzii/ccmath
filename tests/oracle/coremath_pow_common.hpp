#pragma once

#include "coremath_oracle_harness.hpp"

#include <crmath.h>

#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace ccm::test::oracle
{
	template <typename T>
	inline T coremath_pow_reference(T base, T exponent)
	{
		if constexpr (std::is_same_v<T, float>) { return cr_powf(base, exponent); }
		else if constexpr (std::is_same_v<T, double>) { return cr_pow(base, exponent); }
		else { return static_cast<T>(cr_pow(static_cast<double>(base), static_cast<double>(exponent))); }
	}

	// Higher-precision correctly-rounded float reference, used only to cross-check
	// disagreements with the primary float oracle (cr_powf). cr_powf has a fast path
	// that flushes results extremely close to 1.0 to exactly 1.0 in every rounding
	// mode (ignoring directed rounding) and mishandles (-1)^(large even); the double
	// oracle cr_pow does not. We evaluate cr_pow under round-up and round-down, take
	// the round-to-odd double, and cast it to float in the active rounding mode, which
	// yields the correctly rounded float without double rounding.
	inline float coremath_float_truth(float base, float exponent)
	{
		const int active = std::fegetround();
		std::fesetround(FE_UPWARD);
		const double du = cr_pow(static_cast<double>(base), static_cast<double>(exponent));
		std::fesetround(FE_DOWNWARD);
		const double dd = cr_pow(static_cast<double>(base), static_cast<double>(exponent));
		std::fesetround(active);
		double rod = du;
		if (du != dd)
		{
			std::uint64_t bd = 0;
			std::memcpy(&bd, &dd, sizeof(bd));
			rod = (bd & 1U) ? dd : du;
		}
		volatile float result = static_cast<float>(rod); // final rounding in the active mode
		return result;
	}

	// True when the function output disagrees with the primary oracle but is in fact
	// correctly rounded (confirmed by a higher-precision cross-check), i.e. the oracle
	// is wrong for this case. Only float has a higher-precision cross-check available
	// here; cr_pow (double) is trusted directly.
	template <typename T>
	inline bool function_matches_high_precision_truth(T base, T exponent, T actual)
	{
		if constexpr (std::is_same_v<T, float>)
		{
			const float truth = coremath_float_truth(base, exponent);
			return oracle_fp_bits_match(actual, truth);
		}
		else
		{
			(void) base;
			(void) exponent;
			(void) actual;
			return false;
		}
	}

	template <typename T, typename Fn>
	inline std::optional<failure_record<T>> evaluate_case_in_mode(const pow_case<T> & test_case,
																  std::string_view function_name,
																  std::string_view path_name,
																  int rounding_mode,
																  Fn fn,
																  run_summary<T> & summary,
																  std::uint64_t seed		   = 0,
																  std::string_view search_mode = {},
																  std::vector<failure_record<T>> * event_log = nullptr)
	{
		return evaluate_binary_coremath_case_in_mode(
			test_case,
			function_name,
			path_name,
			rounding_mode,
			fn,
			[](T base, T exponent) { return coremath_pow_reference(base, exponent); },
			[](T base, T exponent) { return is_coremath_oracle_case(base, exponent); },
			[](T base, T exponent, T actual) { return function_matches_high_precision_truth(base, exponent, actual); },
			summary,
			seed,
			search_mode,
			"bit-exact mismatch vs CORE-MATH cr_pow",
			event_log);
	}

	template <typename T, typename Fn>
	inline void evaluate_case_all_modes(const pow_case<T> & test_case,
										std::string_view function_name,
										std::string_view path_name,
										const std::vector<int> & rounding_modes,
										Fn fn,
										run_summary<T> & summary,
										std::vector<failure_record<T>> & failures,
										std::uint64_t seed			 = 0,
										std::string_view search_mode = {},
										std::vector<failure_record<T>> * event_log = nullptr)
	{
		evaluate_binary_coremath_case_all_modes(
			test_case,
			function_name,
			path_name,
			rounding_modes,
			fn,
			[](T base, T exponent) { return coremath_pow_reference(base, exponent); },
			[](T base, T exponent) { return is_coremath_oracle_case(base, exponent); },
			[](T base, T exponent, T actual) { return function_matches_high_precision_truth(base, exponent, actual); },
			summary,
			failures,
			seed,
			search_mode,
			"bit-exact mismatch vs CORE-MATH cr_pow",
			event_log);
	}
} // namespace ccm::test::oracle
