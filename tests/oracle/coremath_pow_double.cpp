#include "coremath_pow_cases.hpp"
#include "coremath_pow_common.hpp"

#include <iostream>

namespace
{
	using ccm::test::oracle::campaign_mode;
	using ccm::test::oracle::pow_case;
	using ccm::test::pow_path::validation_path;

	void run_path(validation_path path,
				  const std::vector<pow_case<double>> & cases,
				  const std::vector<int> & rounding_modes,
				  std::uint64_t seed,
				  campaign_mode mode,
				  std::vector<ccm::test::oracle::failure_record<double>> & failures,
				  ccm::test::oracle::run_summary<double> & summary)
	{
		const auto support = ccm::test::pow_path::path_is_supported<double>(path);
		if (!support.supported)
		{
			std::cout << "SKIP path=" << ccm::test::pow_path::path_name(path) << " reason=" << support.skip_reason << '\n';
			return;
		}

		const auto started = std::chrono::steady_clock::now();
		for (const auto & test_case : cases)
		{
			ccm::test::oracle::evaluate_case_all_modes(
				test_case,
				"ccm::pow",
				ccm::test::pow_path::path_name(path),
				rounding_modes,
				[ path ](double base, double exponent) { return ccm::test::pow_path::invoke(path, base, exponent); },
				summary,
				failures,
				seed);
		}
		const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - started);
		const auto report = ccm::test::oracle::make_coremath_campaign_report<double>(
			ccm::test::pow_path::path_name(path), mode, summary, seed, static_cast<std::uint64_t>(elapsed.count()), { "finite-binary64-corpus" }, {});
		const std::string summary_path = "coremath-pow-double-" + report.path + "-summary.json";
		ccm::test::oracle::write_campaign_summary_json(summary_path, report);

		std::cout << "path=" << report.path << " configuration=" << report.configuration_name << " cases=" << report.case_count
				  << " skipped=" << report.skipped_count << " max_ulp=" << report.max_observed_ulp << " failures=" << report.failure_count
				  << " elapsed_ms=" << report.elapsed_ms << " summary=" << summary_path << '\n';
	}
} // namespace

int main(int argc, char ** argv)
{
	const auto mode = ccm::test::oracle::parse_mode(ccm::test::oracle::option_value(argc, argv, "--mode="));
	const auto output_path = ccm::test::oracle::option_value(argc, argv, "--json-output=");
	const std::uint64_t seed =
		ccm::test::oracle::parse_option_or<std::uint64_t>(ccm::test::oracle::option_value(argc, argv, "--seed="),
			[](const std::string & value) { return std::stoull(value); },
			0xC0DEC0FFEEULL);

	auto paths = ccm::test::oracle::parse_paths(argc, argv);
	const auto rounding_modes = ccm::test::oracle::parse_rounding_modes(argc, argv);
	const auto cases = ccm::test::oracle::coremath_cases::build_double_cases(mode, seed);
	std::vector<ccm::test::oracle::failure_record<double>> failures;

	std::cout << "coremath pow<double> campaign mode=" << ccm::test::oracle::mode_name(mode) << " fma=" << ccm::test::oracle::fma_status()
			  << " configuration=" << ccm::test::oracle::configuration_name() << " rounding_modes=" << rounding_modes.size() << '\n';

	for (const validation_path path : paths)
	{
		ccm::test::oracle::run_summary<double> summary;
		run_path(path, cases, rounding_modes, seed, mode, failures, summary);
	}

	if (output_path.has_value()) { ccm::test::oracle::write_failure_json(*output_path, failures); }
	if (output_path.has_value()) { std::cout << "failures json: " << *output_path << '\n'; }

	return failures.empty() ? 0 : 1;
}
