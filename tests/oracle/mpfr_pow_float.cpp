#include "mpfr_pow_common.hpp"
#include "powf_exhaustive_domains.hpp"

#include "tests/utils/math_samples.hpp"
#include "utils/worst_case_samples.hpp"

#include <iostream>

namespace
{
	using ccm::test::oracle::campaign_mode;
	using ccm::test::oracle::pow_case;
	namespace powf_domains = ccm::test::oracle::powf_domains;
	using ccm::test::pow_path::validation_path;

	std::vector<std::string> all_domains()
	{
		return {
			"mantissa-sweep",
			"all-x-for-y",
			"all-y-for-x",
			"exponent-field-sweep",
			"subnormal-x",
			"unit-interval",
			"negative-base",
			"overflow-threshold",
			"underflow-threshold",
			"structured-corpus",
		};
	}

	std::vector<pow_case<float>> build_cases(campaign_mode mode,
											 const std::set<std::string_view> & domain_filter,
											 std::uint64_t seed,
											 std::vector<std::string> & domains_covered,
											 std::vector<std::string> & domains_skipped)
	{
		std::vector<pow_case<float>> cases;
		const bool filter_all = domain_filter.empty();

		auto domain_enabled = [&](std::string_view domain) {
			return filter_all || domain_filter.find(domain) != domain_filter.end();
		};

		auto mark_domain = [&](std::string_view domain, bool enabled) {
			if (enabled) { domains_covered.emplace_back(domain); }
			else { domains_skipped.emplace_back(domain); }
		};

		if (domain_enabled("structured-corpus"))
		{
			mark_domain("structured-corpus", true);
			for (const auto & hard_case : ccm::test::worst_case::kPowFloatHard)
			{
				cases.push_back({ hard_case.base, hard_case.exponent, hard_case.provenance });
			}

			const std::array<float, 10> special_bases = {
				-std::numeric_limits<float>::infinity(),
				-1.0F,
				-0.0F,
				0.0F,
				std::numeric_limits<float>::denorm_min(),
				std::numeric_limits<float>::min(),
				1.0F,
				std::nextafter(1.0F, 0.0F),
				std::numeric_limits<float>::max(),
				std::numeric_limits<float>::infinity(),
			};
			const std::array<float, 9> special_exponents = {
				-std::numeric_limits<float>::infinity(),
				-3.0F,
				-1.0F,
				-0.0F,
				0.0F,
				0.5F,
				2.0F,
				3.0F,
				std::numeric_limits<float>::infinity(),
			};
			for (float base : special_bases)
			{
				for (float exponent : special_exponents)
				{
					cases.push_back({ base, exponent, "special-value matrix" });
				}
			}

			const int bucket_step = (mode == campaign_mode::quick) ? 16 : 1;
			for (int bucket = 0; bucket < 128; bucket += bucket_step)
			{
				const float boundary = 1.0F + static_cast<float>(bucket) / 128.0F;
				cases.push_back({ boundary, 10.0F, "range-reduction boundary: exact bucket edge" });
				cases.push_back({ std::nextafter(boundary, 0.0F), -10.0F, "range-reduction boundary: previous representable" });
				cases.push_back({ std::nextafter(boundary, std::numeric_limits<float>::infinity()), 10.0F, "range-reduction boundary: next representable" });
			}

			const std::size_t random_count = mode == campaign_mode::quick ? 1024 : (mode == campaign_mode::extended ? 8192 : 32768);
			ccm::test::oracle::add_random_cases(cases, seed, random_count, "deterministic random bit-pattern campaign");
		}
		else
		{
			mark_domain("structured-corpus", false);
		}

		const auto add_case = [&](float base, float exponent, const char * provenance) {
			cases.push_back({ base, exponent, provenance });
		};

		const auto filter_copy = domain_filter;
		const std::size_t before = cases.size();
		powf_domains::add_mantissa_sweep(mode, add_case, filter_copy);
		mark_domain("mantissa-sweep", powf_domains::domain_enabled(filter_copy, "mantissa-sweep") && cases.size() > before);

		const std::size_t before_all_x = cases.size();
		powf_domains::add_all_x_for_y(mode, add_case, filter_copy);
		mark_domain("all-x-for-y", powf_domains::domain_enabled(filter_copy, "all-x-for-y") && cases.size() > before_all_x);

		const std::size_t before_all_y = cases.size();
		powf_domains::add_all_y_for_x(mode, add_case, filter_copy);
		mark_domain("all-y-for-x", powf_domains::domain_enabled(filter_copy, "all-y-for-x") && cases.size() > before_all_y);

		const std::size_t before_exp = cases.size();
		powf_domains::add_exponent_field_sweep(mode, add_case, filter_copy);
		mark_domain("exponent-field-sweep", powf_domains::domain_enabled(filter_copy, "exponent-field-sweep") && cases.size() > before_exp);

		const std::size_t before_sub = cases.size();
		powf_domains::add_subnormal_x(mode, add_case, filter_copy);
		mark_domain("subnormal-x", powf_domains::domain_enabled(filter_copy, "subnormal-x") && cases.size() > before_sub);

		if (domain_enabled("unit-interval"))
		{
			mark_domain("unit-interval", true);
			powf_domains::add_unit_interval(mode, add_case, { "mantissa-sweep" });
		}
		else
		{
			mark_domain("unit-interval", false);
		}

		const std::size_t before_neg = cases.size();
		powf_domains::add_negative_base(mode, add_case, filter_copy);
		mark_domain("negative-base", powf_domains::domain_enabled(filter_copy, "negative-base") && cases.size() > before_neg);

		const std::size_t before_ov = cases.size();
		powf_domains::add_overflow_threshold(add_case, filter_copy);
		mark_domain("overflow-threshold", powf_domains::domain_enabled(filter_copy, "overflow-threshold") && cases.size() > before_ov);

		const std::size_t before_un = cases.size();
		powf_domains::add_underflow_threshold(add_case, filter_copy);
		mark_domain("underflow-threshold", powf_domains::domain_enabled(filter_copy, "underflow-threshold") && cases.size() > before_un);

		if (!filter_all)
		{
			for (const auto & domain : all_domains())
			{
				if (domain_filter.find(domain) == domain_filter.end() &&
					std::find(domains_covered.begin(), domains_covered.end(), domain) == domains_covered.end() &&
					std::find(domains_skipped.begin(), domains_skipped.end(), domain) == domains_skipped.end())
				{
					domains_skipped.emplace_back(domain);
				}
			}
		}

		return cases;
	}

	void run_path(validation_path path,
				  const std::vector<pow_case<float>> & cases,
				  const std::vector<int> & rounding_modes,
				  mpfr_prec_t precision,
				  std::uint64_t max_ulp,
				  std::uint64_t target_ulp,
				  std::uint64_t seed,
				  campaign_mode mode,
				  const std::vector<std::string> & domains_covered,
				  const std::vector<std::string> & domains_skipped,
				  std::vector<ccm::test::oracle::failure_record<float>> & failures,
				  ccm::test::oracle::run_summary<float> & summary)
	{
		ccm::test::oracle::run_path_campaign<float>(
			path,
			summary,
			"mpfr-powf-",
			[&](ccm::test::oracle::run_summary<float> & path_summary) {
				for (const int rounding_mode : rounding_modes)
				{
					ccm::test::oracle::ScopedMpfrRoundingMode scope(rounding_mode);
					if (!scope) { continue; }
					for (const auto & test_case : cases)
					{
						if (path == validation_path::generic_modeled_domain &&
							!ccm::test::oracle::is_modeled_generic_pow_case(test_case.base, test_case.exponent))
						{
							continue;
						}
						if (auto failure = ccm::test::oracle::evaluate_case(
								test_case,
								"ccm::powf",
								ccm::test::pow_path::path_name(path),
								path,
								[ path ](float base, float exponent) { return ccm::test::pow_path::invoke(path, base, exponent); },
								precision,
								max_ulp,
								path_summary,
								target_ulp,
								seed))
						{
							failures.push_back(*failure);
						}
					}
				}
			},
			[&](const ccm::test::oracle::run_summary<float> & path_summary, std::uint64_t elapsed_ms) {
				return ccm::test::oracle::make_campaign_report<float>(
					ccm::test::pow_path::path_name(path),
					path,
					mode,
					path_summary,
					seed,
					elapsed_ms,
					domains_covered,
					domains_skipped);
			},
			[&](const auto & report, const std::string & summary_path) {
				std::cout << "path=" << report.path << " configuration=" << report.configuration_name
						  << " rounding_modes=" << rounding_modes.size() << " cases=" << report.case_count
						  << " skipped=" << report.skipped_count
						  << " mpfr_policy_mismatches=" << report.mpfr_policy_mismatch_count
						  << " max_ulp=" << report.max_observed_ulp << " target_ulp=" << target_ulp
						  << " above_target=" << report.above_target_count << " failures=" << report.failure_count
						  << " elapsed_ms=" << report.elapsed_ms << " summary=" << summary_path << '\n';
			});
	}
} // namespace

int main(int argc, char ** argv)
{
	const auto mode = ccm::test::oracle::parse_mode(ccm::test::oracle::option_value(argc, argv, "--mode="));
	const auto output_path = ccm::test::oracle::option_value(argc, argv, "--json-output=");
	const auto domain_raw = ccm::test::oracle::option_value(argc, argv, "--domain=");
	const auto domain_filter = powf_domains::parse_domain_filter(domain_raw);
	const std::uint64_t seed =
		ccm::test::oracle::parse_option_or<std::uint64_t>(ccm::test::oracle::option_value(argc, argv, "--seed="),
			[](const std::string & value) { return std::stoull(value); },
			0xFACEB00CU);
	const std::uint64_t max_ulp =
		ccm::test::oracle::parse_option_or<std::uint64_t>(ccm::test::oracle::option_value(argc, argv, "--max-ulp="),
			[](const std::string & value) { return static_cast<std::uint64_t>(std::stoull(value)); },
			4);
	// Hard ceiling stays at 4 ULP. The correctly-rounded target is 0.5 ULP, which in the
	// integer ULP-distance metric versus the rounded MPFR reference is distance 0.
	const std::uint64_t target_ulp =
		ccm::test::oracle::parse_option_or<std::uint64_t>(ccm::test::oracle::option_value(argc, argv, "--target-ulp="),
			[](const std::string & value) { return static_cast<std::uint64_t>(std::stoull(value)); },
			0);
	const mpfr_prec_t precision =
		ccm::test::oracle::parse_option_or<mpfr_prec_t>(ccm::test::oracle::option_value(argc, argv, "--oracle-precision="),
			[](const std::string & value) { return static_cast<mpfr_prec_t>(std::stoul(value)); },
			192);

	auto paths = ccm::test::oracle::parse_paths(argc, argv);
	if (ccm::test::oracle::has_flag(argc, argv, "--include-generic-modeled-path"))
	{
		paths.push_back(validation_path::generic_modeled_domain);
	}

	const auto rounding_modes = ccm::test::oracle::parse_rounding_modes(argc, argv);
	mpfr_set_default_rounding_mode(MPFR_RNDN);

	std::vector<std::string> domains_covered;
	std::vector<std::string> domains_skipped;
	const auto cases = build_cases(mode, domain_filter, seed, domains_covered, domains_skipped);
	std::vector<ccm::test::oracle::failure_record<float>> failures;

	std::cout << "mpfr powf campaign mode=" << ccm::test::oracle::mode_name(mode) << " fma=" << ccm::test::oracle::fma_status()
			  << " configuration=" << ccm::test::oracle::configuration_name() << " cases=" << cases.size()
			  << " rounding_modes=" << rounding_modes.size() << '\n';

	for (const validation_path path : paths)
	{
		ccm::test::oracle::run_summary<float> summary;
		run_path(path, cases, rounding_modes, precision, max_ulp, target_ulp, seed, mode, domains_covered, domains_skipped, failures, summary);
	}

	if (output_path.has_value()) { ccm::test::oracle::write_failure_json(*output_path, failures); }
	if (output_path.has_value()) { std::cout << "failures json: " << *output_path << '\n'; }

	return failures.empty() ? 0 : 1;
}
