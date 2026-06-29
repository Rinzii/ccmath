#pragma once

#include "oracle_campaign_common.hpp"
#include "utils/fenv_fixture.hpp"

#include <optional>
#include <string_view>
#include <vector>

namespace ccm::test::oracle
{
	template <typename T, typename ActualFn, typename ReferenceFn, typename EligibilityFn, typename TruthFn>
	inline std::optional<failure_record<T>> evaluate_binary_coremath_case_in_mode(const pow_case<T> & test_case,
																				  std::string_view function_name,
																				  std::string_view path_name,
																				  int rounding_mode,
																				  ActualFn actual_fn,
																				  ReferenceFn reference_fn,
																				  EligibilityFn is_reference_case_fn,
																				  TruthFn matches_truth_fn,
																				  run_summary<T> & summary,
																				  std::uint64_t seed			 = 0,
																				  std::string_view search_mode	 = {},
																				  std::string_view mismatch_note = "bit-exact mismatch vs CORE-MATH reference",
																				  std::vector<failure_record<T>> * event_log = nullptr)
	{
		if (!is_reference_case_fn(test_case.base, test_case.exponent))
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

		const T expected = reference_fn(test_case.base, test_case.exponent);
		const T actual	 = actual_fn(test_case.base, test_case.exponent);

		if (oracle_fp_bits_match(actual, expected))
		{
			return std::nullopt;
		}

		if (matches_truth_fn(test_case.base, test_case.exponent, actual))
		{
			++summary.oracle_corrected_count;
			if (event_log != nullptr)
			{
				event_log->push_back(make_failure_record(function_name,
														 path_name,
														 test_case.provenance,
														 test_case.base,
														 test_case.exponent,
														 actual,
														 expected,
														 0,
														 ccm::test::RoundingModeName(rounding_mode),
														 0,
														 seed,
														 search_mode,
														 "ccm matches higher-precision truth, CORE-MATH oracle disagrees",
														 "coremath_oracle_corrected"));
			}
			return std::nullopt;
		}

		++summary.failure_count;
		if (summary.max_observed_ulp == 0)
		{
			record_worst_case(summary, 1, test_case.base, test_case.exponent, actual, expected);
		}

		auto record = make_failure_record(function_name,
										  path_name,
										  test_case.provenance,
										  test_case.base,
										  test_case.exponent,
										  actual,
										  expected,
										  0,
										  ccm::test::RoundingModeName(rounding_mode),
										  0,
										  seed,
										  search_mode,
										  mismatch_note,
										  "coremath_bit_mismatch");
		if (event_log != nullptr)
		{
			event_log->push_back(record);
		}
		return record;
	}

	template <typename T, typename ActualFn, typename ReferenceFn, typename EligibilityFn, typename TruthFn>
	inline void evaluate_binary_coremath_case_all_modes(const pow_case<T> & test_case,
														std::string_view function_name,
														std::string_view path_name,
														const std::vector<int> & rounding_modes,
														ActualFn actual_fn,
														ReferenceFn reference_fn,
														EligibilityFn is_reference_case_fn,
														TruthFn matches_truth_fn,
														run_summary<T> & summary,
														std::vector<failure_record<T>> & failures,
														std::uint64_t seed						   = 0,
														std::string_view search_mode			   = {},
														std::string_view mismatch_note			   = "bit-exact mismatch vs CORE-MATH reference",
														std::vector<failure_record<T>> * event_log = nullptr)
	{
		for (const int rounding_mode : rounding_modes)
		{
			if (auto failure = evaluate_binary_coremath_case_in_mode(test_case,
																	 function_name,
																	 path_name,
																	 rounding_mode,
																	 actual_fn,
																	 reference_fn,
																	 is_reference_case_fn,
																	 matches_truth_fn,
																	 summary,
																	 seed,
																	 search_mode,
																	 mismatch_note,
																	 event_log))
			{
				if (event_log == nullptr)
				{
					failures.push_back(*failure);
				}
			}
		}
	}
} // namespace ccm::test::oracle
