#include "mpfr_pow_common.hpp"

#include "utils/math_samples.hpp"

#include <iostream>

namespace
{
	using ccm::test::oracle::campaign_mode;
	using ccm::test::oracle::pow_case;
	using ccm::test::pow_path::validation_path;

	std::vector<pow_case<double>> build_cases(campaign_mode mode, std::uint64_t seed)
	{
		std::vector<pow_case<double>> cases;
		for (const auto & legacy_case : ccm::test::worst_case::kPowDoubleHard)
		{
			cases.push_back({ legacy_case.base, legacy_case.exponent, legacy_case.provenance });
		}

		const std::array<double, 10> special_bases = {
			-std::numeric_limits<double>::infinity(),
			-1.0,
			-0.0,
			0.0,
			std::numeric_limits<double>::denorm_min(),
			std::numeric_limits<double>::min(),
			1.0,
			std::nextafter(1.0, 0.0),
			std::numeric_limits<double>::max(),
			std::numeric_limits<double>::infinity(),
		};
		const std::array<double, 9> special_exponents = {
			-std::numeric_limits<double>::infinity(),
			-3.0,
			-1.0,
			-0.0,
			0.0,
			0.5,
			2.0,
			3.0,
			std::numeric_limits<double>::infinity(),
		};
		for (double base : special_bases)
		{
			for (double exponent : special_exponents)
			{
				cases.push_back({ base, exponent, "special-value matrix" });
			}
		}

		for (const double base : {
				 std::nextafter(1.0, 0.0),
				 1.0,
				 std::nextafter(1.0, std::numeric_limits<double>::infinity()),
				 std::nextafter(-1.0, -std::numeric_limits<double>::infinity()),
				 -1.0,
				 std::nextafter(-1.0, 0.0),
			 })
		{
			for (double exponent : { -3.5, -1.5, -0.5, 0.5, 1.5, 3.5, 0x1.fffffffffffffp52, 0x1.0p53 })
			{
				cases.push_back({ base, exponent, "near +/-1 boundary campaign" });
			}
		}

		const int bucket_step = (mode == campaign_mode::quick) ? 16 : 1;
		for (int bucket = 0; bucket < 128; bucket += bucket_step)
		{
			const double boundary = 1.0 + static_cast<double>(bucket) / 128.0;
			cases.push_back({ boundary, -10.0, "range-reduction boundary: exact bucket edge" });
			cases.push_back({ std::nextafter(boundary, 0.0), -10.0, "range-reduction boundary: previous representable" });
			cases.push_back({ std::nextafter(boundary, std::numeric_limits<double>::infinity()), 10.0, "range-reduction boundary: next representable" });
		}

		for (const int exp2_exponent : { -1022, -10, -1, 0, 1, 10, 1023 })
		{
			const double base = std::ldexp(1.0, exp2_exponent);
			for (double exponent : { -1.0, -0.5, 2.0, 3.0 })
			{
				cases.push_back({ base, exponent, "power-of-two campaign" });
				cases.push_back({ std::nextafter(base, 0.0), exponent, "nextafter neighborhood below power-of-two" });
				cases.push_back({ std::nextafter(base, std::numeric_limits<double>::infinity()), exponent, "nextafter neighborhood above power-of-two" });
			}
		}

		for (double exponent : { -3.0, -2.0, -1.0, 1.0, 2.0, 3.0, 0x1.fffffffffffffp52, 0x1.0p53 })
		{
			cases.push_back({ -2.0, exponent, "negative base with integer exponent campaign" });
		}
		for (double exponent : { 0.5, 1.5, 3.5, std::nextafter(1.0, 2.0), std::nextafter(2.0, 1.0) })
		{
			cases.push_back({ -2.0, exponent, "negative base near non-integer exponent boundary" });
		}

		for (const auto exponent : { 1023.0, std::nextafter(1024.0, 0.0), 1024.0, 1025.0 })
		{
			cases.push_back({ 2.0, exponent, "overflow-threshold campaign" });
		}
		for (const auto exponent : { -1074.0, -1075.0, -1076.0 })
		{
			cases.push_back({ 2.0, exponent, "underflow-threshold campaign" });
		}

		const std::size_t random_count = mode == campaign_mode::quick ? 512 : (mode == campaign_mode::extended ? 4096 : 16384);
		ccm::test::oracle::add_random_cases(cases, seed, random_count, "deterministic random bit-pattern campaign");
		return cases;
	}

	void run_path(validation_path path,
				  const std::vector<pow_case<double>> & cases,
				  mpfr_prec_t precision,
				  std::uint64_t max_ulp,
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
			if (path == validation_path::generic_modeled_domain &&
				!ccm::test::oracle::is_modeled_generic_pow_case(test_case.base, test_case.exponent))
			{
				continue;
			}
			if (auto failure = ccm::test::oracle::evaluate_case(
					test_case,
					"ccm::pow",
					ccm::test::pow_path::path_name(path),
					path,
					[ path ](double base, double exponent) { return ccm::test::pow_path::invoke(path, base, exponent); },
					precision,
					max_ulp,
					summary,
					seed))
			{
				failures.push_back(*failure);
			}
		}
		const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - started);
		const auto report = ccm::test::oracle::make_campaign_report<double>(
			ccm::test::pow_path::path_name(path), path, mode, summary, seed, static_cast<std::uint64_t>(elapsed.count()), { "structured-binary64-corpus" }, {});
		const std::string summary_path = "mpfr-pow-double-" + report.path + "-summary.json";
		ccm::test::oracle::write_campaign_summary_json(summary_path, report);

		std::cout << "path=" << report.path << " configuration=" << report.configuration_name << " cases=" << report.case_count
				  << " skipped=" << report.skipped_count << " mpfr_policy_mismatches=" << report.mpfr_policy_mismatch_count
				  << " max_ulp=" << report.max_observed_ulp << " failures=" << report.failure_count << " elapsed_ms=" << report.elapsed_ms
				  << " summary=" << summary_path << '\n';
	}
} // namespace

int main(int argc, char ** argv)
{
	const auto mode = ccm::test::oracle::parse_mode(ccm::test::oracle::option_value(argc, argv, "--mode="));
	const auto output_path = ccm::test::oracle::option_value(argc, argv, "--json-output=");
	if (ccm::test::oracle::has_flag(argc, argv, "--include-generic-modeled-path"))
	{
		std::cout << "warning: --include-generic-modeled-path is deprecated, use --path=generic_modeled_domain\n";
	}
	const std::uint64_t seed =
		ccm::test::oracle::parse_option_or<std::uint64_t>(ccm::test::oracle::option_value(argc, argv, "--seed="),
			[](const std::string & value) { return std::stoull(value); },
			0xC0DEC0FFEEULL);
	const std::uint64_t max_ulp =
		ccm::test::oracle::parse_option_or<std::uint64_t>(ccm::test::oracle::option_value(argc, argv, "--max-ulp="),
			[](const std::string & value) { return static_cast<std::uint64_t>(std::stoull(value)); },
			4);
	const mpfr_prec_t precision =
		ccm::test::oracle::parse_option_or<mpfr_prec_t>(ccm::test::oracle::option_value(argc, argv, "--oracle-precision="),
			[](const std::string & value) { return static_cast<mpfr_prec_t>(std::stoul(value)); },
			256);

	auto paths = ccm::test::oracle::parse_paths(argc, argv);
	if (ccm::test::oracle::has_flag(argc, argv, "--include-generic-modeled-path"))
	{
		paths.push_back(validation_path::generic_modeled_domain);
	}

	const auto cases = build_cases(mode, seed);
	std::vector<ccm::test::oracle::failure_record<double>> failures;

	std::cout << "mpfr pow<double> campaign mode=" << ccm::test::oracle::mode_name(mode) << " fma=" << ccm::test::oracle::fma_status()
			  << " configuration=" << ccm::test::oracle::configuration_name() << '\n';

	for (const validation_path path : paths)
	{
		ccm::test::oracle::run_summary<double> summary;
		run_path(path, cases, precision, max_ulp, seed, mode, failures, summary);
	}

	if (output_path.has_value()) { ccm::test::oracle::write_failure_json(*output_path, failures); }
	if (output_path.has_value()) { std::cout << "failures json: " << *output_path << '\n'; }

	return failures.empty() ? 0 : 1;
}
