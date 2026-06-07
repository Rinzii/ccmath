#pragma once

#include "oracle_campaign_common.hpp"

#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace ccm::test::oracle::powf_domains
{
	inline std::set<std::string_view> parse_domain_filter(const std::optional<std::string> & raw_domains)
	{
		std::set<std::string_view> domains;
		if (!raw_domains.has_value() || raw_domains->empty()) { return domains; }

		std::string_view view = *raw_domains;
		while (!view.empty())
		{
			const std::size_t comma = view.find(',');
			const std::string_view token = view.substr(0, comma);
			if (!token.empty()) { domains.insert(token); }
			if (comma == std::string_view::npos) { break; }
			view.remove_prefix(comma + 1);
		}
		return domains;
	}

	inline bool domain_enabled(const std::set<std::string_view> & filter, std::string_view domain)
	{
		return filter.empty() || filter.find(domain) != filter.end();
	}

	template <typename AddCaseFn>
	inline void add_mantissa_sweep(campaign_mode mode, AddCaseFn add_case, const std::set<std::string_view> & filter)
	{
		if (!domain_enabled(filter, "mantissa-sweep")) { return; }

		for (float exponent : { 2.0F, 0.5F, -1.0F, 3.14159265F })
		{
			const std::uint32_t step = (mode == campaign_mode::quick) ? (1u << 15)
				: (mode == campaign_mode::extended ? (1u << 8) : 1u);
			for (std::uint32_t mantissa = 0; mantissa < (1u << 23); mantissa += step)
			{
				add_case(ccm::support::bit_cast<float>(0x3f800000u | mantissa), exponent, "reduced-domain exhaustive [1,2) mantissa sweep");
			}
		}
	}

	template <typename AddCaseFn>
	inline void add_all_x_for_y(campaign_mode mode, AddCaseFn add_case, const std::set<std::string_view> & filter)
	{
		if (!domain_enabled(filter, "all-x-for-y")) { return; }

		const std::uint32_t step = (mode == campaign_mode::quick) ? (1u << 20)
			: (mode == campaign_mode::extended ? (1u << 12) : (1u << 8));
		for (float exponent : { 2.0F, -1.0F, 0.5F })
		{
			for (std::uint32_t bits = 0; bits < 0x7f800000u; bits += step)
			{
				add_case(ccm::support::bit_cast<float>(bits), exponent, "all-x-for-selected-y");
			}
		}
	}

	template <typename AddCaseFn>
	inline void add_all_y_for_x(campaign_mode mode, AddCaseFn add_case, const std::set<std::string_view> & filter)
	{
		if (!domain_enabled(filter, "all-y-for-x")) { return; }

		const std::uint32_t step = (mode == campaign_mode::quick) ? (1u << 20)
			: (mode == campaign_mode::extended ? (1u << 12) : (1u << 8));
		for (float base : { 1.25F, 2.0F, 0.5F, -2.0F })
		{
			for (std::uint32_t bits = 0; bits < 0x7f800000u; bits += step)
			{
				add_case(base, ccm::support::bit_cast<float>(bits), "all-y-for-selected-x");
			}
		}
	}

	template <typename AddCaseFn>
	inline void add_exponent_field_sweep(campaign_mode mode, AddCaseFn add_case, const std::set<std::string_view> & filter)
	{
		if (!domain_enabled(filter, "exponent-field-sweep")) { return; }

		for (const float base : { 1.0F, -1.0F, 2.0F, -2.0F, 0x1.0p-126F, 0x1.0p-149F })
		{
			for (const std::uint32_t raw_exponent : { 0x00000000u, 0x3f800000u, 0x40000000u, 0x40400000u, 0xbf800000u })
			{
				add_case(base, ccm::support::bit_cast<float>(raw_exponent), "selected-x sweep across exact exponent anchors");
			}
		}

		if (mode == campaign_mode::release)
		{
			for (std::uint32_t exp_field = 0; exp_field < 256u; ++exp_field)
			{
				const std::uint32_t raw = (exp_field << 23) | 0x400000u;
				add_case(1.25F, ccm::support::bit_cast<float>(raw), "mantissa-fixed exponent-field sweep");
			}
		}
	}

	template <typename AddCaseFn>
	inline void add_subnormal_x(campaign_mode mode, AddCaseFn add_case, const std::set<std::string_view> & filter)
	{
		if (!domain_enabled(filter, "subnormal-x")) { return; }

		const std::uint32_t step = (mode == campaign_mode::quick) ? (1u << 15)
			: (mode == campaign_mode::extended ? (1u << 8) : 1u);
		for (std::uint32_t bits = 1; bits < 0x00800000u; bits += step)
		{
			add_case(ccm::support::bit_cast<float>(bits), 2.0F, "subnormal-base sweep");
			add_case(ccm::support::bit_cast<float>(bits | 0x80000000u), 2.0F, "negative subnormal-base sweep");
		}
	}

	template <typename AddCaseFn>
	inline void add_unit_interval(campaign_mode mode, AddCaseFn add_case, const std::set<std::string_view> & filter)
	{
		if (!domain_enabled(filter, "unit-interval")) { return; }
		add_mantissa_sweep(mode, add_case, { "mantissa-sweep" });
	}

	template <typename AddCaseFn>
	inline void add_negative_base([[maybe_unused]] campaign_mode mode, AddCaseFn add_case, const std::set<std::string_view> & filter)
	{
		if (!domain_enabled(filter, "negative-base")) { return; }

		for (float exponent : { -3.0F, -2.0F, -1.0F, 1.0F, 2.0F, 3.0F, 8388607.0F, 8388608.0F, 8388609.0F, 16777216.0F })
		{
			add_case(-1.0F, exponent, "integer-parity threshold campaign");
		}
		for (float exponent : { 0.5F, 1.5F, 3.5F, std::nextafter(1.0F, 2.0F) })
		{
			add_case(-2.0F, exponent, "negative-base non-integer boundary campaign");
		}
	}

	template <typename AddCaseFn>
	inline void add_overflow_threshold(AddCaseFn add_case, const std::set<std::string_view> & filter)
	{
		if (!domain_enabled(filter, "overflow-threshold")) { return; }

		for (float exponent : { 126.0F, 127.0F, 128.0F, 129.0F })
		{
			add_case(2.0F, exponent, "overflow-threshold campaign");
		}
	}

	template <typename AddCaseFn>
	inline void add_underflow_threshold(AddCaseFn add_case, const std::set<std::string_view> & filter)
	{
		if (!domain_enabled(filter, "underflow-threshold")) { return; }

		for (float exponent : { -150.0F, -151.0F, -152.0F })
		{
			add_case(2.0F, exponent, "underflow-threshold campaign");
		}
	}
} // namespace ccm::test::oracle::powf_domains
