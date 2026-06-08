#include "coremath_pow_cases.hpp"
#include "coremath_pow_common.hpp"

#include <iostream>

namespace
{
	using ccm::test::oracle::campaign_mode;
	using ccm::test::oracle::pow_case;
	namespace powf_domains = ccm::test::oracle::powf_domains;
	using ccm::test::pow_path::validation_path;

	void run_path(validation_path path,
				  const std::vector<pow_case<float>>& cases,
				  const std::vector<int>& rounding_modes,
				  std::uint64_t seed,
				  campaign_mode mode,
				  const std::vector<std::string>& domains_covered,
				  const std::vector<std::string>& domains_skipped,
				  std::vector<ccm::test::oracle::failure_record<float>>& events,
				  ccm::test::oracle::run_summary<float>& summary)
	{
		ccm::test::oracle::run_path_campaign<float>(
			path,
			summary,
			"coremath-powf-",
			[&](ccm::test::oracle::run_summary<float>& path_summary)
			{
				for (const auto& test_case : cases)
				{
					ccm::test::oracle::evaluate_case_all_modes(
						test_case,
						"ccm::powf",
						ccm::test::pow_path::path_name(path),
						rounding_modes,
						[path](float base, float exponent) { return ccm::test::pow_path::invoke(path, base, exponent); },
						path_summary,
						events,
						seed,
						{},
						&events);
				}
			},
			[&](const ccm::test::oracle::run_summary<float>& path_summary, std::uint64_t elapsed_ms)
			{
				return ccm::test::oracle::make_coremath_campaign_report<float>(
					ccm::test::pow_path::path_name(path), mode, path_summary, seed, elapsed_ms, domains_covered, domains_skipped);
			},
			[&](const auto& report, const std::string& summary_path)
			{
				std::cout << "path=" << report.path << " configuration=" << report.configuration_name << " cases=" << report.case_count
						  << " skipped=" << report.skipped_count << " max_ulp=" << report.max_observed_ulp << " failures=" << report.failure_count
						  << " elapsed_ms=" << report.elapsed_ms << " summary=" << summary_path << '\n';
			});
	}
} // namespace

int main(int argc, char** argv)
{
	const auto mode			 = ccm::test::oracle::parse_mode(ccm::test::oracle::option_value(argc, argv, "--mode="));
	const auto output_path	 = ccm::test::oracle::resolve_event_log_path(argc, argv);
	const auto domain_raw	 = ccm::test::oracle::option_value(argc, argv, "--domain=");
	const auto domain_filter = powf_domains::parse_domain_filter(domain_raw);
	const std::uint64_t seed = ccm::test::oracle::parse_option_or<std::uint64_t>(
		ccm::test::oracle::option_value(argc, argv, "--seed="), [](const std::string& value) { return std::stoull(value); }, 0xFACEB00CU);

	auto paths				  = ccm::test::oracle::parse_paths(argc, argv);
	const auto rounding_modes = ccm::test::oracle::parse_rounding_modes(argc, argv);

	std::vector<std::string> domains_covered;
	std::vector<std::string> domains_skipped;
	const auto cases = ccm::test::oracle::coremath_cases::build_float_cases(mode, domain_filter, seed, domains_covered, domains_skipped);
	std::vector<ccm::test::oracle::failure_record<float>> events;

	std::cout << "coremath powf campaign mode=" << ccm::test::oracle::mode_name(mode) << " fma=" << ccm::test::oracle::fma_status()
			  << " configuration=" << ccm::test::oracle::configuration_name() << " cases=" << cases.size() << " rounding_modes=" << rounding_modes.size()
			  << '\n';

	for (const validation_path path : paths)
	{
		ccm::test::oracle::run_summary<float> summary;
		run_path(path, cases, rounding_modes, seed, mode, domains_covered, domains_skipped, events, summary);
	}

	if (output_path.has_value()) { ccm::test::oracle::write_failure_json(*output_path, events); }
	if (output_path.has_value())
	{
		std::cout << "oracle event log: " << *output_path << " events=" << events.size()
				  << " hard_failures=" << (ccm::test::oracle::has_hard_oracle_failure(events) ? "yes" : "no") << '\n';
	}

	return ccm::test::oracle::has_hard_oracle_failure(events) ? 1 : 0;
}
