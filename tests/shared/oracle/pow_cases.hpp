#pragma once

#include "oracle_campaign_common.hpp"
#include "powf_exhaustive_domains.hpp"
#include "utils/worst_case_samples.hpp"

#include <algorithm>
#include <array>
#include <limits>
#include <set>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

namespace ccm::test::oracle::pow_cases
{
	inline std::vector<std::string> all_powf_domains()
	{
		return {
			"mantissa-sweep", "all-x-for-y",   "all-y-for-x",		 "exponent-field-sweep", "subnormal-x",
			"unit-interval",  "negative-base", "overflow-threshold", "underflow-threshold",	 "structured-corpus",
		};
	}

	template <typename EligibilityFn>
	void append_double_cases(std::vector<pow_case<double>>& cases, campaign_mode mode, std::uint64_t seed, EligibilityFn eligible)
	{
		auto maybe_add = [&](double base, double exponent, const char* provenance)
		{
			if (eligible(base, exponent)) { cases.push_back({ base, exponent, provenance }); }
		};

		for (const auto& legacy_case : ccm::test::worst_case::kPowDoubleHard) { maybe_add(legacy_case.base, legacy_case.exponent, legacy_case.provenance); }

		const std::array special_bases = {
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
		constexpr std::array special_exponents = {
			-std::numeric_limits<double>::infinity(), -3.0, -1.0, -0.0, 0.0, 0.5, 2.0, 3.0, std::numeric_limits<double>::infinity(),
		};
		for (double base : special_bases)
		{
			for (double exponent : special_exponents) { maybe_add(base, exponent, "special-value matrix"); }
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
				maybe_add(base, exponent, "near +/-1 boundary campaign");
			}
		}

		const int bucket_step = (mode == campaign_mode::quick) ? 16 : 1;
		for (int bucket = 0; bucket < 128; bucket += bucket_step)
		{
			const double boundary = 1.0 + static_cast<double>(bucket) / 128.0;
			maybe_add(boundary, -10.0, "range-reduction boundary: exact bucket edge");
			maybe_add(std::nextafter(boundary, 0.0), -10.0, "range-reduction boundary: previous representable");
			maybe_add(std::nextafter(boundary, std::numeric_limits<double>::infinity()), 10.0, "range-reduction boundary: next representable");
		}

		for (const int exp2_exponent : { -1022, -10, -1, 0, 1, 10, 1023 })
		{
			const double base = std::ldexp(1.0, exp2_exponent);
			for (double exponent : { -1.0, -0.5, 2.0, 3.0 })
			{
				maybe_add(base, exponent, "power-of-two campaign");
				maybe_add(std::nextafter(base, 0.0), exponent, "nextafter neighborhood below power-of-two");
				maybe_add(std::nextafter(base, std::numeric_limits<double>::infinity()), exponent, "nextafter neighborhood above power-of-two");
			}
		}

		for (double exponent : { -3.0, -2.0, -1.0, 1.0, 2.0, 3.0, 0x1.fffffffffffffp52, 0x1.0p53 })
		{
			maybe_add(-2.0, exponent, "negative base with integer exponent campaign");
		}
		for (double exponent : { 0.5, 1.5, 3.5, std::nextafter(1.0, 2.0), std::nextafter(2.0, 1.0) })
		{
			maybe_add(-2.0, exponent, "negative base near non-integer exponent boundary");
		}

		for (const auto exponent : { 1023.0, std::nextafter(1024.0, 0.0), 1024.0, 1025.0 }) { maybe_add(2.0, exponent, "overflow-threshold campaign"); }
		for (const auto exponent : { -1074.0, -1075.0, -1076.0 }) { maybe_add(2.0, exponent, "underflow-threshold campaign"); }

		const std::size_t random_count = mode == campaign_mode::quick ? 512 : (mode == campaign_mode::extended ? 4096 : 16384);
		std::vector<pow_case<double>> random_cases;
		add_random_cases(random_cases, seed, random_count, "deterministic random bit-pattern campaign");
		for (auto& test_case : random_cases)
		{
			if (eligible(test_case.base, test_case.exponent)) { cases.push_back(std::move(test_case)); }
		}
	}

	template <typename EligibilityFn>
	std::vector<pow_case<double>> build_double_cases(campaign_mode mode, std::uint64_t seed, EligibilityFn eligible)
	{
		std::vector<pow_case<double>> cases;
		append_double_cases(cases, mode, seed, eligible);
		return cases;
	}

	inline std::vector<pow_case<double>> build_double_cases(campaign_mode mode, std::uint64_t seed)
	{
		return build_double_cases(mode, seed, [](double, double) { return true; });
	}

	template <typename EligibilityFn>
	std::vector<pow_case<float>> build_float_cases(campaign_mode mode,
												   const std::set<std::string_view>& domain_filter,
												   std::uint64_t seed,
												   std::vector<std::string>& domains_covered,
												   std::vector<std::string>& domains_skipped,
												   EligibilityFn eligible)
	{
		namespace powf_domains = ccm::test::oracle::powf_domains;
		std::vector<pow_case<float>> cases;
		const bool filter_all = domain_filter.empty();

		auto domain_enabled = [&](std::string_view domain) { return filter_all || domain_filter.find(domain) != domain_filter.end(); };

		auto mark_domain = [&](std::string_view domain, bool enabled)
		{
			if (enabled) { domains_covered.emplace_back(domain); }
			else
			{
				domains_skipped.emplace_back(domain);
			}
		};

		auto maybe_add = [&](float base, float exponent, const char* provenance)
		{
			if (eligible(base, exponent)) { cases.push_back({ base, exponent, provenance }); }
		};

		if (domain_enabled("structured-corpus"))
		{
			mark_domain("structured-corpus", true);
			for (const auto& hard_case : ccm::test::worst_case::kPowFloatHard) { maybe_add(hard_case.base, hard_case.exponent, hard_case.provenance); }

			const std::array special_bases = {
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
			constexpr std::array special_exponents = {
				-std::numeric_limits<float>::infinity(), -3.0F, -1.0F, -0.0F, 0.0F, 0.5F, 2.0F, 3.0F, std::numeric_limits<float>::infinity(),
			};
			for (float base : special_bases)
			{
				for (float exponent : special_exponents) { maybe_add(base, exponent, "special-value matrix"); }
			}

			const int bucket_step = (mode == campaign_mode::quick) ? 16 : 1;
			for (int bucket = 0; bucket < 128; bucket += bucket_step)
			{
				const float boundary = 1.0F + static_cast<float>(bucket) / 128.0F;
				maybe_add(boundary, 10.0F, "range-reduction boundary: exact bucket edge");
				maybe_add(std::nextafter(boundary, 0.0F), -10.0F, "range-reduction boundary: previous representable");
				maybe_add(std::nextafter(boundary, std::numeric_limits<float>::infinity()), 10.0F, "range-reduction boundary: next representable");
			}

			const std::size_t random_count = mode == campaign_mode::quick ? 1024 : (mode == campaign_mode::extended ? 8192 : 32768);
			std::vector<pow_case<float>> random_cases;
			add_random_cases(random_cases, seed, random_count, "deterministic random bit-pattern campaign");
			for (auto& test_case : random_cases) { maybe_add(test_case.base, test_case.exponent, test_case.provenance.c_str()); }
		}
		else
		{
			mark_domain("structured-corpus", false);
		}

		const auto add_case = [&](float base, float exponent, const char* provenance) { maybe_add(base, exponent, provenance); };

		const auto filter_copy	 = domain_filter;
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
			for (const auto& domain : all_powf_domains())
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

	inline std::vector<pow_case<float>> build_float_cases(campaign_mode mode,
														  const std::set<std::string_view>& domain_filter,
														  std::uint64_t seed,
														  std::vector<std::string>& domains_covered,
														  std::vector<std::string>& domains_skipped)
	{
		return build_float_cases(mode, domain_filter, seed, domains_covered, domains_skipped, [](float, float) { return true; });
	}
} // namespace ccm::test::oracle::pow_cases
