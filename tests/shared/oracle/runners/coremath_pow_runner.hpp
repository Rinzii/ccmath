#pragma once

#include "oracle/coremath_pow_cases.hpp"
#include "oracle/coremath_pow_common.hpp"
#include "oracle/powf_exhaustive_domains.hpp"

#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace ccm::test::oracle::pow_coremath
{
	namespace powf_domains = ccm::test::oracle::powf_domains;
	using ccm::test::pow_path::validation_path;

	template <typename T> struct traits;

	template <> struct traits<double>
	{
		static constexpr std::string_view function_name	 = "ccm::pow";
		static constexpr std::string_view summary_prefix = "coremath-pow-double-";
		static constexpr std::uint64_t default_seed		 = 0xC0DEC0FFEEULL;
		static constexpr std::string_view corpus_label	 = "finite-binary64-corpus";

		static std::vector<pow_case<double>>
		build_cases(campaign_mode mode, std::set<std::string_view> const &, std::uint64_t seed, std::vector<std::string> &, std::vector<std::string> &)
		{
			return coremath_cases::build_double_cases(mode, seed);
		}
	};

	template <> struct traits<float>
	{
		static constexpr std::string_view function_name	 = "ccm::powf";
		static constexpr std::string_view summary_prefix = "coremath-powf-";
		static constexpr std::uint64_t default_seed		 = 0xFACEB00CU;
		static constexpr std::string_view corpus_label	 = "finite-binary32-corpus";

		static std::vector<pow_case<float>> build_cases(campaign_mode mode,
														std::set<std::string_view> const & domain_filter,
														std::uint64_t seed,
														std::vector<std::string> & domains_covered,
														std::vector<std::string> & domains_skipped)
		{
			return coremath_cases::build_float_cases(mode, domain_filter, seed, domains_covered, domains_skipped);
		}
	};

	template <typename T>
	inline void run_path(validation_path path,
						 std::vector<pow_case<T>> const & cases,
						 std::vector<int> const & rounding_modes,
						 std::uint64_t seed,
						 campaign_mode mode,
						 std::vector<std::string> const & domains_covered,
						 std::vector<std::string> const & domains_skipped,
						 std::vector<failure_record<T>> & events,
						 run_summary<T> & summary)
	{
		run_path_campaign<T>(
			path,
			summary,
			traits<T>::summary_prefix,
			[&](run_summary<T> & path_summary) {
				for (auto const & test_case : cases)
				{
					evaluate_case_all_modes(
						test_case,
						traits<T>::function_name,
						pow_path::path_name(path),
						rounding_modes,
						[path](T base, T exponent) { return pow_path::invoke(path, base, exponent); },
						path_summary,
						events,
						seed,
						{},
						&events);
				}
			},
			[&](run_summary<T> const & path_summary, std::uint64_t elapsed_ms) {
				return make_coremath_campaign_report<T>(pow_path::path_name(path), mode, path_summary, seed, elapsed_ms, domains_covered, domains_skipped);
			},
			[&](auto const & report, std::string const & summary_path) {
				std::cout << "path=" << report.path << " configuration=" << report.configuration_name << " cases=" << report.case_count
						  << " skipped=" << report.skipped_count << " max_ulp=" << report.max_observed_ulp << " failures=" << report.failure_count
						  << " elapsed_ms=" << report.elapsed_ms << " summary=" << summary_path << '\n';
			});
	}

	template <typename T> inline int run_campaign(int argc, char ** argv)
	{
		auto const mode			 = parse_mode(option_value(argc, argv, "--mode="));
		auto const output_path	 = resolve_event_log_path(argc, argv);
		auto const domain_raw	 = option_value(argc, argv, "--domain=");
		auto const domain_filter = powf_domains::parse_domain_filter(domain_raw);
		std::uint64_t const seed = parse_option_or<std::uint64_t>(
			option_value(argc, argv, "--seed="), [](std::string const & value) { return std::stoull(value); }, traits<T>::default_seed);

		auto paths				  = parse_paths(argc, argv);
		auto const rounding_modes = parse_rounding_modes(argc, argv);

		std::vector<std::string> domains_covered;
		std::vector<std::string> domains_skipped;
		if constexpr (std::is_same_v<T, double>)
		{
			domains_covered = { std::string(traits<T>::corpus_label) };
		}
		auto const cases = traits<T>::build_cases(mode, domain_filter, seed, domains_covered, domains_skipped);
		std::vector<failure_record<T>> events;

		std::cout << "coremath pow campaign type=" << scalar_type_name<T>() << " mode=" << mode_name(mode) << " fma=" << fma_status()
				  << " configuration=" << configuration_name() << " cases=" << cases.size() << " rounding_modes=" << rounding_modes.size() << '\n';

		for (validation_path path : paths)
		{
			run_summary<T> summary;
			run_path(path, cases, rounding_modes, seed, mode, domains_covered, domains_skipped, events, summary);
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

} // namespace ccm::test::oracle::pow_coremath
