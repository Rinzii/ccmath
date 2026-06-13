#pragma once

#include "oracle_campaign_common.hpp"
#include "pow_cases.hpp"

namespace ccm::test::oracle::coremath_cases
{
	inline std::vector<std::string> all_powf_domains()
	{ return pow_cases::all_powf_domains(); }

	inline std::vector<pow_case<double>> build_double_cases(campaign_mode mode, std::uint64_t seed)
	{ return pow_cases::build_double_cases(mode, seed, is_coremath_oracle_case<double>); }

	inline std::vector<pow_case<float>> build_float_cases(campaign_mode mode,
														  const std::set<std::string_view>& domain_filter,
														  std::uint64_t seed,
														  std::vector<std::string>& domains_covered,
														  std::vector<std::string>& domains_skipped)
	{ return pow_cases::build_float_cases(mode, domain_filter, seed, domains_covered, domains_skipped, is_coremath_oracle_case<float>); }
} // namespace ccm::test::oracle::coremath_cases
