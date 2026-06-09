#pragma once

#include "ccmath/internal/support/bits.hpp"
#include "mpfr_pow_common.hpp"
#include "tests/utils/worst_case_samples.hpp"

#include <cmath>
#include <limits>
#include <random>
#include <string>
#include <string_view>
#include <vector>

namespace ccm::test::oracle::adversarial
{
	template <typename T>
	using pow_case = ccm::test::oracle::pow_case<T>;

	template <typename T>
	inline void add_near_one(std::vector<pow_case<T>>& cases)
	{
		for (T exponent : { static_cast<T>(-3.5), static_cast<T>(-0.5), static_cast<T>(0.5), static_cast<T>(3.5), static_cast<T>(10) })
		{
			cases.push_back({ static_cast<T>(1), exponent, "near-one" });
			cases.push_back({ std::nextafter(static_cast<T>(1), static_cast<T>(0)), exponent, "near-one below" });
			cases.push_back({ std::nextafter(static_cast<T>(1), std::numeric_limits<T>::infinity()), exponent, "near-one above" });
		}
	}

	template <typename T>
	inline void add_near_negative_one(std::vector<pow_case<T>>& cases)
	{
		for (T exponent : { static_cast<T>(2), static_cast<T>(3), static_cast<T>(4), static_cast<T>(3.5), static_cast<T>(0.5) })
		{
			cases.push_back({ static_cast<T>(-1), exponent, "near-negative-one" });
			cases.push_back({ std::nextafter(static_cast<T>(-1), -std::numeric_limits<T>::infinity()), exponent, "near-negative-one below" });
			cases.push_back({ std::nextafter(static_cast<T>(-1), static_cast<T>(0)), exponent, "near-negative-one above" });
		}
	}

	template <typename T>
	inline void add_near_zero(std::vector<pow_case<T>>& cases)
	{
		for (T exponent : { static_cast<T>(-1), static_cast<T>(0.5), static_cast<T>(2), static_cast<T>(3) })
		{
			cases.push_back({ static_cast<T>(0), exponent, "near-zero" });
			cases.push_back({ std::numeric_limits<T>::denorm_min(), exponent, "near-zero denorm min" });
			cases.push_back({ -std::numeric_limits<T>::denorm_min(), exponent, "near-zero negative denorm min" });
		}
	}

	template <typename T>
	inline void add_near_power_of_two(std::vector<pow_case<T>>& cases)
	{
		for (int exp2 : { -10, -1, 0, 1, 10, 20 })
		{
			const T base = std::ldexp(static_cast<T>(1), exp2);
			for (T exponent : { static_cast<T>(-2), static_cast<T>(0.5), static_cast<T>(2), static_cast<T>(3) })
			{
				cases.push_back({ base, exponent, "near-power-of-two" });
				cases.push_back({ std::nextafter(base, static_cast<T>(0)), exponent, "near-power-of-two below" });
				cases.push_back({ std::nextafter(base, std::numeric_limits<T>::infinity()), exponent, "near-power-of-two above" });
			}
		}
	}

	template <typename T>
	inline void add_near_integer_exponent(std::vector<pow_case<T>>& cases)
	{
		for (T base : { static_cast<T>(1.25), static_cast<T>(2), static_cast<T>(-2), static_cast<T>(0.5) })
		{
			for (T delta : { static_cast<T>(-0.5), static_cast<T>(-1e-6), static_cast<T>(1e-6), static_cast<T>(0.5) })
			{
				for (int integer : { -3, -1, 0, 1, 3, 7 }) { cases.push_back({ base, static_cast<T>(integer) + delta, "near-integer-exponent" }); }
			}
		}
	}

	template <typename T>
	inline void add_negative_base_parity(std::vector<pow_case<T>>& cases)
	{
		for (T exponent : { static_cast<T>(2), static_cast<T>(3), static_cast<T>(4), static_cast<T>(3.5), static_cast<T>(0.5) })
		{
			cases.push_back({ static_cast<T>(-2), exponent, "negative-base-parity" });
		}
	}

	template <typename T>
	inline void add_overflow_boundary(std::vector<pow_case<T>>& cases)
	{
		if constexpr (std::is_same_v<T, float>)
		{
			for (T exponent : { static_cast<T>(126), static_cast<T>(127), static_cast<T>(128), static_cast<T>(129) })
			{
				cases.push_back({ static_cast<T>(2), exponent, "overflow-boundary" });
			}
		}
		else
		{
			for (T exponent : { static_cast<T>(1023), static_cast<T>(1024), static_cast<T>(1025) })
			{
				cases.push_back({ static_cast<T>(2), exponent, "overflow-boundary" });
			}
		}
	}

	template <typename T>
	inline void add_underflow_boundary(std::vector<pow_case<T>>& cases)
	{
		if constexpr (std::is_same_v<T, float>)
		{
			for (T exponent : { static_cast<T>(-150), static_cast<T>(-151), static_cast<T>(-152) })
			{
				cases.push_back({ static_cast<T>(2), exponent, "underflow-boundary" });
			}
		}
		else
		{
			for (T exponent : { static_cast<T>(-1074), static_cast<T>(-1075), static_cast<T>(-1076) })
			{
				cases.push_back({ static_cast<T>(2), exponent, "underflow-boundary" });
			}
		}
	}

	template <typename T>
	inline void add_subnormal_output(std::vector<pow_case<T>>& cases, std::uint64_t seed, std::size_t count)
	{
		std::mt19937_64 rng(seed);
		for (std::size_t i = 0; i < count; ++i)
		{
			const T base	 = static_cast<T>(0.5) + static_cast<T>(rng() % 1000) / static_cast<T>(10000);
			const T exponent = static_cast<T>(-120) - static_cast<T>(rng() % 40);
			cases.push_back({ base, exponent, "subnormal-output" });
		}
	}

	template <typename T>
	inline void add_table_boundary(std::vector<pow_case<T>>& cases)
	{
		for (int bucket = 0; bucket < 128; ++bucket)
		{
			const T boundary = static_cast<T>(1) + static_cast<T>(bucket) / static_cast<T>(128);
			cases.push_back({ boundary, static_cast<T>(10), "table-boundary" });
			cases.push_back({ std::nextafter(boundary, static_cast<T>(0)), static_cast<T>(-10), "table-boundary below edge" });
			cases.push_back({ std::nextafter(boundary, std::numeric_limits<T>::infinity()), static_cast<T>(10), "table-boundary above edge" });
		}
	}

	template <typename T>
	inline void add_field_mutation(std::string_view mode, std::uint64_t seed, std::size_t count, std::vector<pow_case<T>>& cases)
	{
		std::mt19937_64 rng(seed);
		for (std::size_t i = 0; i < count; ++i)
		{
			if constexpr (std::is_same_v<T, float>)
			{
				std::uint32_t base_bits = static_cast<std::uint32_t>(rng());
				std::uint32_t exp_bits	= static_cast<std::uint32_t>(rng());
				if (mode == "exponent-field-mutation" || mode == "exponent-mutation")
				{
					base_bits = (base_bits & 0x807fffffu) | ((static_cast<std::uint32_t>(rng()) & 0xffu) << 23);
				}
				else
				{
					base_bits ^= static_cast<std::uint32_t>(1u << (rng() % 23));
				}
				cases.push_back({ ccm::support::bit_cast<T>(base_bits), ccm::support::bit_cast<T>(exp_bits), std::string(mode) });
			}
			else
			{
				std::uint64_t base_bits = rng();
				std::uint64_t exp_bits	= rng();
				if (mode == "exponent-field-mutation" || mode == "exponent-mutation")
				{
					base_bits = (base_bits & 0x800fffffffffffffull) | ((rng() & 0x7ffull) << 52);
				}
				else
				{
					base_bits ^= (1ull << (rng() % 52));
				}
				cases.push_back({ ccm::support::bit_cast<T>(base_bits), ccm::support::bit_cast<T>(exp_bits), std::string(mode) });
			}
		}
	}

	template <typename T>
	inline std::vector<pow_case<T>> build_search_cases(std::string_view mode, std::uint64_t seed, std::size_t count)
	{
		std::vector<pow_case<T>> cases;
		if (mode == "random-bit-pattern")
		{
			ccm::test::oracle::add_random_cases(cases, seed, count, "adversarial random bit-pattern search");
			return cases;
		}
		if (mode == "table-boundary-search" || mode == "range-reduction-boundary-search" || mode == "table-boundary")
		{
			add_table_boundary(cases);
			return cases;
		}
		if (mode == "overflow-boundary-search" || mode == "overflow-boundary")
		{
			add_overflow_boundary(cases);
			return cases;
		}
		if (mode == "underflow-boundary-search" || mode == "underflow-boundary")
		{
			add_underflow_boundary(cases);
			return cases;
		}
		if (mode == "negative-base-integer-boundary-search" || mode == "negative-base-parity")
		{
			add_negative_base_parity(cases);
			return cases;
		}
		if (mode == "nextafter-neighborhood")
		{
			for (const auto& hard_case : ccm::test::worst_case::kPowDoubleHard)
			{
				const T base	 = static_cast<T>(hard_case.base);
				const T exponent = static_cast<T>(hard_case.exponent);
				cases.push_back({ base, exponent, "seed corpus case" });
				cases.push_back({ std::nextafter(base, std::numeric_limits<T>::infinity()), exponent, "nextafter walk on base upward" });
				cases.push_back({ std::nextafter(base, static_cast<T>(0)), exponent, "nextafter walk on base downward" });
				cases.push_back({ base, std::nextafter(exponent, std::numeric_limits<T>::infinity()), "nextafter walk on exponent upward" });
				cases.push_back({ base, std::nextafter(exponent, static_cast<T>(0)), "nextafter walk on exponent downward" });
			}
			return cases;
		}
		if (mode == "near-one")
		{
			add_near_one(cases);
			return cases;
		}
		if (mode == "near-negative-one")
		{
			add_near_negative_one(cases);
			return cases;
		}
		if (mode == "near-zero")
		{
			add_near_zero(cases);
			return cases;
		}
		if (mode == "near-power-of-two")
		{
			add_near_power_of_two(cases);
			return cases;
		}
		if (mode == "near-integer-exponent")
		{
			add_near_integer_exponent(cases);
			return cases;
		}
		if (mode == "subnormal-output")
		{
			add_subnormal_output(cases, seed, count);
			return cases;
		}
		if (mode == "exponent-field-mutation" || mode == "mantissa-field-mutation" || mode == "exponent-mutation" || mode == "mantissa-mutation")
		{
			add_field_mutation<T>(mode, seed, count, cases);
			return cases;
		}

		ccm::test::oracle::add_random_cases(cases, seed, count, "fallback random search");
		return cases;
	}
} // namespace ccm::test::oracle::adversarial
