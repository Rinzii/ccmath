#include "mpfr_pow_common.hpp"
#include "pow_cases.hpp"
#include "powf_exhaustive_domains.hpp"

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
				  mpfr_prec_t precision,
				  std::uint64_t max_ulp,
				  std::uint64_t target_ulp,
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
			"mpfr-powf-",
			[&](ccm::test::oracle::run_summary<float>& path_summary)
			{
				for (const int rounding_mode : rounding_modes)
				{
					ccm::test::oracle::ScopedMpfrRoundingMode scope(rounding_mode);
					if (!scope) { continue; }
					for (const auto& test_case : cases)
					{
						if (path == validation_path::generic_modeled_domain &&
							!ccm::test::oracle::is_modeled_generic_pow_case(test_case.base, test_case.exponent))
						{
							continue;
						}
						(void)ccm::test::oracle::evaluate_case(
							test_case,
							"ccm::powf",
							ccm::test::pow_path::path_name(path),
							path,
							[path](float base, float exponent) { return ccm::test::pow_path::invoke(path, base, exponent); },
							precision,
							max_ulp,
							path_summary,
							target_ulp,
							seed,
							{},
							&events);
					}
				}
			},
			[&](const ccm::test::oracle::run_summary<float>& path_summary, std::uint64_t elapsed_ms)
			{
				return ccm::test::oracle::make_campaign_report<float>(
					ccm::test::pow_path::path_name(path), path, mode, path_summary, seed, elapsed_ms, domains_covered, domains_skipped);
			},
			[&](const auto& report, const std::string& summary_path)
			{
				std::cout << "path=" << report.path << " configuration=" << report.configuration_name << " rounding_modes=" << rounding_modes.size()
						  << " cases=" << report.case_count << " skipped=" << report.skipped_count
						  << " mpfr_policy_mismatches=" << report.mpfr_policy_mismatch_count << " max_ulp=" << report.max_observed_ulp
						  << " target_ulp=" << target_ulp << " above_target=" << report.above_target_count << " failures=" << report.failure_count
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
	const std::uint64_t max_ulp = ccm::test::oracle::parse_option_or<std::uint64_t>(
		ccm::test::oracle::option_value(argc, argv, "--max-ulp="), [](const std::string& value) { return static_cast<std::uint64_t>(std::stoull(value)); }, 4);
	// Hard ceiling stays at 4 ULP. The correctly-rounded target is 0.5 ULP, which in the
	// integer ULP-distance metric versus the rounded MPFR reference is distance 0.
	const std::uint64_t target_ulp = ccm::test::oracle::parse_option_or<std::uint64_t>(
		ccm::test::oracle::option_value(argc, argv, "--target-ulp="),
		[](const std::string& value) { return static_cast<std::uint64_t>(std::stoull(value)); },
		0);
	const mpfr_prec_t precision = ccm::test::oracle::parse_option_or<mpfr_prec_t>(
		ccm::test::oracle::option_value(argc, argv, "--oracle-precision="),
		[](const std::string& value) { return static_cast<mpfr_prec_t>(std::stoul(value)); },
		192);

	auto paths = ccm::test::oracle::parse_paths(argc, argv);
	if (ccm::test::oracle::has_flag(argc, argv, "--include-generic-modeled-path")) { paths.push_back(validation_path::generic_modeled_domain); }

	const auto rounding_modes = ccm::test::oracle::parse_rounding_modes(argc, argv);
	mpfr_set_default_rounding_mode(MPFR_RNDN);

	std::vector<std::string> domains_covered;
	std::vector<std::string> domains_skipped;
	const auto cases = ccm::test::oracle::pow_cases::build_float_cases(mode, domain_filter, seed, domains_covered, domains_skipped);
	std::vector<ccm::test::oracle::failure_record<float>> events;

	std::cout << "mpfr powf campaign mode=" << ccm::test::oracle::mode_name(mode) << " fma=" << ccm::test::oracle::fma_status()
			  << " configuration=" << ccm::test::oracle::configuration_name() << " cases=" << cases.size() << " rounding_modes=" << rounding_modes.size()
			  << '\n';

	for (const validation_path path : paths)
	{
		ccm::test::oracle::run_summary<float> summary;
		run_path(path, cases, rounding_modes, precision, max_ulp, target_ulp, seed, mode, domains_covered, domains_skipped, events, summary);
	}

	if (output_path.has_value()) { ccm::test::oracle::write_failure_json(*output_path, events); }
	if (output_path.has_value())
	{
		std::cout << "oracle event log: " << *output_path << " events=" << events.size()
				  << " hard_failures=" << (ccm::test::oracle::has_hard_oracle_failure(events) ? "yes" : "no") << '\n';
	}

	return ccm::test::oracle::has_hard_oracle_failure(events) ? 1 : 0;
}
