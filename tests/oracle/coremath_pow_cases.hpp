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

namespace ccm::test::oracle::coremath_cases
{
	inline std::vector<pow_case<double>> build_double_cases(campaign_mode mode, std::uint64_t seed)
	{
		std::vector<pow_case<double>> cases;
		for (const auto & legacy_case : ccm::test::worst_case::kPowDoubleHard)
		{
			if (!is_coremath_oracle_case(legacy_case.base, legacy_case.exponent)) { continue; }
			cases.push_back({ legacy_case.base, legacy_case.exponent, legacy_case.provenance });
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
				if (!is_coremath_oracle_case(base, exponent)) { continue; }
				cases.push_back({ base, exponent, "near +/-1 boundary campaign" });
			}
		}

		const int bucket_step = (mode == campaign_mode::quick) ? 16 : 1;
		for (int bucket = 0; bucket < 128; bucket += bucket_step)
		{
			const double boundary = 1.0 + static_cast<double>(bucket) / 128.0;
			const std::array<std::tuple<double, double, std::string_view>, 3> boundary_cases = {
				std::tuple{ boundary, -10.0, std::string_view{ "range-reduction boundary: exact bucket edge" } },
				std::tuple{ std::nextafter(boundary, 0.0), -10.0, std::string_view{ "range-reduction boundary: previous representable" } },
				std::tuple{ std::nextafter(boundary, std::numeric_limits<double>::infinity()), 10.0, std::string_view{ "range-reduction boundary: next representable" } },
			};
			for (const auto & entry : boundary_cases)
			{
				const double base		 = std::get<0>(entry);
				const double exponent	 = std::get<1>(entry);
				const std::string_view label = std::get<2>(entry);
				if (!is_coremath_oracle_case(base, exponent)) { continue; }
				cases.push_back({ base, exponent, std::string(label) });
			}
		}

		for (const int exp2_exponent : { -1022, -10, -1, 0, 1, 10, 1023 })
		{
			const double base = std::ldexp(1.0, exp2_exponent);
			for (double exponent : { -1.0, -0.5, 2.0, 3.0 })
			{
				for (double b : { base, std::nextafter(base, 0.0), std::nextafter(base, std::numeric_limits<double>::infinity()) })
				{
					if (!is_coremath_oracle_case(b, exponent)) { continue; }
					cases.push_back({ b, exponent, "power-of-two campaign" });
				}
			}
		}

		for (double exponent : { -3.0, -2.0, -1.0, 1.0, 2.0, 3.0, 0x1.fffffffffffffp52, 0x1.0p53 })
		{
			if (!is_coremath_oracle_case(-2.0, exponent)) { continue; }
			cases.push_back({ -2.0, exponent, "negative base with integer exponent campaign" });
		}

		for (const auto exponent : { 1023.0, std::nextafter(1024.0, 0.0), 1024.0, 1025.0 })
		{
			if (!is_coremath_oracle_case(2.0, exponent)) { continue; }
			cases.push_back({ 2.0, exponent, "overflow-threshold campaign" });
		}
		for (const auto exponent : { -1074.0, -1075.0, -1076.0 })
		{
			if (!is_coremath_oracle_case(2.0, exponent)) { continue; }
			cases.push_back({ 2.0, exponent, "underflow-threshold campaign" });
		}

		const std::size_t random_count = mode == campaign_mode::quick ? 512 : (mode == campaign_mode::extended ? 4096 : 16384);
		std::vector<pow_case<double>> random_cases;
		add_random_cases(random_cases, seed, random_count, "deterministic random bit-pattern campaign");
		for (auto & test_case : random_cases)
		{
			if (!is_coremath_oracle_case(test_case.base, test_case.exponent)) { continue; }
			cases.push_back(std::move(test_case));
		}
		return cases;
	}

	inline std::vector<std::string> all_powf_domains()
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

	inline std::vector<pow_case<float>> build_float_cases(campaign_mode mode,
														  const std::set<std::string_view> & domain_filter,
														  std::uint64_t seed,
														  std::vector<std::string> & domains_covered,
														  std::vector<std::string> & domains_skipped)
	{
		namespace powf_domains = ccm::test::oracle::powf_domains;
		std::vector<pow_case<float>> cases;
		const bool filter_all = domain_filter.empty();

		auto domain_enabled = [&](std::string_view domain) {
			return filter_all || domain_filter.find(domain) != domain_filter.end();
		};

		auto mark_domain = [&](std::string_view domain, bool enabled) {
			if (enabled) { domains_covered.emplace_back(domain); }
			else { domains_skipped.emplace_back(domain); }
		};

		auto append_if_finite = [&](float base, float exponent, const char * provenance) {
			if (!is_coremath_oracle_case(base, exponent)) { return; }
			cases.push_back({ base, exponent, provenance });
		};

		if (domain_enabled("structured-corpus"))
		{
			mark_domain("structured-corpus", true);
			for (const auto & hard_case : ccm::test::worst_case::kPowFloatHard)
			{
				append_if_finite(hard_case.base, hard_case.exponent, hard_case.provenance);
			}

			const int bucket_step = (mode == campaign_mode::quick) ? 16 : 1;
			for (int bucket = 0; bucket < 128; bucket += bucket_step)
			{
				const float boundary = 1.0F + static_cast<float>(bucket) / 128.0F;
				append_if_finite(boundary, 10.0F, "range-reduction boundary: exact bucket edge");
				append_if_finite(std::nextafter(boundary, 0.0F), -10.0F, "range-reduction boundary: previous representable");
				append_if_finite(std::nextafter(boundary, std::numeric_limits<float>::infinity()), 10.0F, "range-reduction boundary: next representable");
			}

			const std::size_t random_count = mode == campaign_mode::quick ? 1024 : (mode == campaign_mode::extended ? 8192 : 32768);
			std::vector<pow_case<float>> random_cases;
			add_random_cases(random_cases, seed, random_count, "deterministic random bit-pattern campaign");
			for (auto & test_case : random_cases)
			{
				append_if_finite(test_case.base, test_case.exponent, test_case.provenance.c_str());
			}
		}
		else
		{
			mark_domain("structured-corpus", false);
		}

		const auto add_case = [&](float base, float exponent, const char * provenance) {
			append_if_finite(base, exponent, provenance);
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
			for (const auto & domain : all_powf_domains())
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
} // namespace ccm::test::oracle::coremath_cases
