#include "mpfr_pow_common.hpp"
#include "pow_cases.hpp"

#include <iostream>

namespace
{
	using ccm::test::oracle::campaign_mode;
	using ccm::test::oracle::pow_case;
	using ccm::test::pow_path::validation_path;

	void run_path(validation_path path,
				  const std::vector<pow_case<double>>& cases,
				  const std::vector<int>& rounding_modes,
				  mpfr_prec_t precision,
				  std::uint64_t max_ulp,
				  std::uint64_t target_ulp,
				  std::uint64_t seed,
				  campaign_mode mode,
				  std::vector<ccm::test::oracle::failure_record<double>>& events,
				  ccm::test::oracle::run_summary<double>& summary)
	{
		ccm::test::oracle::run_path_campaign<double>(
			path,
			summary,
			"mpfr-pow-double-",
			[&](ccm::test::oracle::run_summary<double>& path_summary)
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
							"ccm::pow",
							ccm::test::pow_path::path_name(path),
							path,
							[path](double base, double exponent) { return ccm::test::pow_path::invoke(path, base, exponent); },
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
			[&](const ccm::test::oracle::run_summary<double>& path_summary, std::uint64_t elapsed_ms)
			{
				return ccm::test::oracle::make_campaign_report<double>(
					ccm::test::pow_path::path_name(path), path, mode, path_summary, seed, elapsed_ms, { "structured-binary64-corpus" }, {});
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
	const auto mode		   = ccm::test::oracle::parse_mode(ccm::test::oracle::option_value(argc, argv, "--mode="));
	const auto output_path = ccm::test::oracle::resolve_event_log_path(argc, argv);
	if (ccm::test::oracle::has_flag(argc, argv, "--include-generic-modeled-path"))
	{
		std::cout << "warning: --include-generic-modeled-path is deprecated, use --path=generic_modeled_domain\n";
	}
	const std::uint64_t seed = ccm::test::oracle::parse_option_or<std::uint64_t>(
		ccm::test::oracle::option_value(argc, argv, "--seed="), [](const std::string& value) { return std::stoull(value); }, 0xC0DEC0FFEEULL);
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
		256);

	auto paths = ccm::test::oracle::parse_paths(argc, argv);
	if (ccm::test::oracle::has_flag(argc, argv, "--include-generic-modeled-path")) { paths.push_back(validation_path::generic_modeled_domain); }

	const auto rounding_modes = ccm::test::oracle::parse_rounding_modes(argc, argv);
	mpfr_set_default_rounding_mode(MPFR_RNDN);

	const auto cases = ccm::test::oracle::pow_cases::build_double_cases(mode, seed);
	std::vector<ccm::test::oracle::failure_record<double>> events;

	std::cout << "mpfr pow<double> campaign mode=" << ccm::test::oracle::mode_name(mode) << " fma=" << ccm::test::oracle::fma_status()
			  << " configuration=" << ccm::test::oracle::configuration_name() << " rounding_modes=" << rounding_modes.size() << '\n';

	for (const validation_path path : paths)
	{
		ccm::test::oracle::run_summary<double> summary;
		run_path(path, cases, rounding_modes, precision, max_ulp, target_ulp, seed, mode, events, summary);
	}

	ccm::test::oracle::print_hard_failures(events);
	if (output_path.has_value()) { ccm::test::oracle::write_failure_json(*output_path, events); }
	if (output_path.has_value())
	{
		std::cout << "oracle event log: " << *output_path << " events=" << events.size()
				  << " hard_failures=" << (ccm::test::oracle::has_hard_oracle_failure(events) ? "yes" : "no") << '\n';
	}

	return ccm::test::oracle::has_hard_oracle_failure(events) ? 1 : 0;
}
