#pragma once

#include "oracle_campaign_common.hpp"

#include "utils/fenv_fixture.hpp"

#include <crmath.h>

#include <cmath>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace ccm::test::oracle
{
	template <typename T>
	inline T coremath_pow_reference(T base, T exponent)
	{
		if constexpr (std::is_same_v<T, float>) { return cr_powf(base, exponent); }
		else if constexpr (std::is_same_v<T, double>) { return cr_pow(base, exponent); }
		else { return static_cast<T>(cr_pow(static_cast<double>(base), static_cast<double>(exponent))); }
	}

	template <typename T, typename Fn>
	inline std::optional<failure_record<T>> evaluate_case_in_mode(const pow_case<T> & test_case,
																  std::string_view function_name,
																  std::string_view path_name,
																  int rounding_mode,
																  Fn fn,
																  run_summary<T> & summary,
																  std::uint64_t seed		   = 0,
																  std::string_view search_mode = {})
	{
		if (!is_coremath_oracle_case(test_case.base, test_case.exponent))
		{
			++summary.skipped_count;
			return std::nullopt;
		}

		++summary.case_count;

		ccm::test::ForceRoundingMode force(rounding_mode);
		if (!force)
		{
			++summary.skipped_count;
			--summary.case_count;
			return std::nullopt;
		}

		const T expected = coremath_pow_reference(test_case.base, test_case.exponent);
		const T actual	 = fn(test_case.base, test_case.exponent);

		if (oracle_fp_bits_match(actual, expected)) { return std::nullopt; }

		++summary.failure_count;
		if (summary.max_observed_ulp == 0)
		{
			summary.max_observed_ulp = 1;
			summary.worst_base		 = test_case.base;
			summary.worst_exponent	 = test_case.exponent;
			summary.worst_actual	 = actual;
			summary.worst_expected	 = expected;
		}

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
			0,
			ccm::test::RoundingModeName(rounding_mode),
			0,
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
			"bit-exact mismatch vs CORE-MATH cr_pow",
		};
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
										std::string_view search_mode = {})
	{
		for (int mode : rounding_modes)
		{
			if (auto failure = evaluate_case_in_mode(test_case, function_name, path_name, mode, fn, summary, seed, search_mode))
			{
				failures.push_back(*failure);
			}
		}
	}
} // namespace ccm::test::oracle
