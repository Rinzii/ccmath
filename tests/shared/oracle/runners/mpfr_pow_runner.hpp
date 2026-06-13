#pragma once

#include "oracle/mpfr_pow_common.hpp"
#include "oracle/pow_cases.hpp"
#include "oracle/powf_exhaustive_domains.hpp"

#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace ccm::test::oracle::pow_mpfr
{
	namespace powf_domains = ccm::test::oracle::powf_domains;
	using ccm::test::pow_path::validation_path;

	template <typename T>
	struct traits;

	template <>
	struct traits<double>
	{
		static constexpr std::string_view function_name = "ccm::pow";
		static constexpr std::string_view summary_prefix = "mpfr-pow-double-";
		static constexpr std::uint64_t default_seed = 0xC0DEC0FFEEULL;
		static constexpr mpfr_prec_t default_precision = 256;
		static constexpr std::string_view corpus_label = "structured-binary64-corpus";

		static std::vector<pow_case<double>> build_cases(campaign_mode mode,
														   std::set<std::string_view> const&,
														   std::uint64_t seed,
														   std::vector<std::string>&,
														   std::vector<std::string>&)
		{ return pow_cases::build_double_cases(mode, seed); }
	};

	template <>
	struct traits<float>
	{
		static constexpr std::string_view function_name = "ccm::powf";
		static constexpr std::string_view summary_prefix = "mpfr-powf-";
		static constexpr std::uint64_t default_seed = 0xFACEB00CU;
		static constexpr mpfr_prec_t default_precision = 192;
		static constexpr std::string_view corpus_label = "structured-binary32-corpus";

		static std::vector<pow_case<float>> build_cases(campaign_mode mode,
														 std::set<std::string_view> const& domain_filter,
														 std::uint64_t seed,
														 std::vector<std::string>& domains_covered,
														 std::vector<std::string>& domains_skipped)
		{ return pow_cases::build_float_cases(mode, domain_filter, seed, domains_covered, domains_skipped); }
	};

	template <typename T>
	inline void run_path(validation_path path,
						 std::vector<pow_case<T>> const& cases,
						 std::vector<int> const& rounding_modes,
						 mpfr_prec_t precision,
						 std::uint64_t max_ulp,
						 std::uint64_t target_ulp,
						 std::uint64_t seed,
						 campaign_mode mode,
						 std::vector<std::string> const& domains_covered,
						 std::vector<std::string> const& domains_skipped,
						 std::vector<failure_record<T>>& events,
						 run_summary<T>& summary)
	{
		run_path_campaign<T>(
			path,
			summary,
			traits<T>::summary_prefix,
			[&](run_summary<T>& path_summary)
			{
				for (int rounding_mode : rounding_modes)
				{
					ScopedMpfrRoundingMode scope(rounding_mode);
					if (!scope) { continue; }
					for (auto const& test_case : cases)
					{
						if (path == validation_path::generic_modeled_domain && !is_modeled_generic_pow_case(test_case.base, test_case.exponent))
						{
							continue;
						}
						(void)evaluate_case(
							test_case,
							traits<T>::function_name,
							pow_path::path_name(path),
							path,
							[path](T base, T exponent) { return pow_path::invoke(path, base, exponent); },
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
			[&](run_summary<T> const& path_summary, std::uint64_t elapsed_ms)
			{
				return make_campaign_report<T>(
					pow_path::path_name(path), path, mode, path_summary, seed, elapsed_ms, domains_covered, domains_skipped);
			},
			[&](auto const& report, std::string const& summary_path)
			{
				std::cout << "path=" << report.path << " configuration=" << report.configuration_name << " rounding_modes=" << rounding_modes.size()
						  << " cases=" << report.case_count << " skipped=" << report.skipped_count
						  << " mpfr_policy_mismatches=" << report.mpfr_policy_mismatch_count << " max_ulp=" << report.max_observed_ulp
						  << " target_ulp=" << target_ulp << " above_target=" << report.above_target_count << " failures=" << report.failure_count
						  << " elapsed_ms=" << report.elapsed_ms << " summary=" << summary_path << '\n';
			});
	}

	template <typename T>
	inline int run_campaign(int argc, char** argv)
	{
		auto const mode = parse_mode(option_value(argc, argv, "--mode="));
		auto const output_path = resolve_event_log_path(argc, argv);
		auto const domain_raw = option_value(argc, argv, "--domain=");
		auto const domain_filter = powf_domains::parse_domain_filter(domain_raw);
		std::uint64_t const seed = parse_option_or<std::uint64_t>(
			option_value(argc, argv, "--seed="), [](std::string const& value) { return std::stoull(value); }, traits<T>::default_seed);
		std::uint64_t const max_ulp = parse_option_or<std::uint64_t>(
			option_value(argc, argv, "--max-ulp="), [](std::string const& value) { return static_cast<std::uint64_t>(std::stoull(value)); }, 4);
		std::uint64_t const target_ulp = parse_option_or<std::uint64_t>(
			option_value(argc, argv, "--target-ulp="),
			[](std::string const& value) { return static_cast<std::uint64_t>(std::stoull(value)); },
			0);
		mpfr_prec_t const precision = parse_option_or<mpfr_prec_t>(
			option_value(argc, argv, "--oracle-precision="),
			[](std::string const& value) { return static_cast<mpfr_prec_t>(std::stoul(value)); },
			traits<T>::default_precision);

		auto paths = parse_paths(argc, argv);
		if (has_flag(argc, argv, "--include-generic-modeled-path"))
		{
			std::cout << "warning: --include-generic-modeled-path is deprecated, use --path=generic_modeled_domain\n";
			paths.push_back(validation_path::generic_modeled_domain);
		}

		auto const rounding_modes = parse_rounding_modes(argc, argv);
		mpfr_set_default_rounding_mode(MPFR_RNDN);

		std::vector<std::string> domains_covered;
		std::vector<std::string> domains_skipped;
		if constexpr (std::is_same_v<T, double>)
		{
			domains_covered = { std::string(traits<T>::corpus_label) };
		}
		auto const cases = traits<T>::build_cases(mode, domain_filter, seed, domains_covered, domains_skipped);
		std::vector<failure_record<T>> events;

		std::cout << "mpfr pow campaign type=" << scalar_type_name<T>() << " mode=" << mode_name(mode) << " fma=" << fma_status()
				  << " configuration=" << configuration_name() << " cases=" << cases.size() << " rounding_modes=" << rounding_modes.size() << '\n';

		for (validation_path path : paths)
		{
			run_summary<T> summary;
			run_path(path, cases, rounding_modes, precision, max_ulp, target_ulp, seed, mode, domains_covered, domains_skipped, events, summary);
		}

		print_hard_failures(events);
		if (output_path.has_value())
		{
			write_failure_json(*output_path, events);
			std::cout << "oracle event log: " << *output_path << " events=" << events.size()
					  << " hard_failures=" << (has_hard_oracle_failure(events) ? "yes" : "no") << '\n';
		}

		return has_hard_oracle_failure(events) ? 1 : 0;
	}

} // namespace ccm::test::oracle::pow_mpfr
