/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <cstdint>
#include <type_traits>

namespace ccm::helpers
{

	/**
	 * @brief
	 * @tparam To
	 * @tparam From
	 * @param src
	 * @return
	 */
	template<class To, class From>
	std::enable_if_t<
		sizeof(To) == sizeof(From) &&
			std::is_trivially_copyable_v<From> &&
			std::is_trivially_copyable_v<To>,
		To>
	inline constexpr bit_cast(const From& src) noexcept
    {
		static_assert(std::is_trivially_constructible_v<To>,
					  "This implementation additionally requires "
					  "destination type to be trivially constructible");

        return __builtin_bit_cast(To, src);
    }

	inline constexpr std::uint32_t get_exponent_of_double(double x) noexcept
	{
		// Reinterpret the binary representation of x as an std::uint64_t
		std::uint64_t bits = bit_cast<std::uint64_t>(x);

		// Extract the exponent bits (bits 62-52) and remove bias (1023)
		std::uint32_t exponent = static_cast<std::uint32_t>((bits >> 52) & 0x7FF) - 1023;

		return exponent;
	}



	/**
	 * @brief Helper function to get the top 16-bits of a double.
	 * @param x Double to get the bits from.
	 * @return
	 */
	inline constexpr std::uint32_t top16_bits_of_double(double x) noexcept
	{
		return bit_cast<std::uint64_t>(x) >> 48;
	}

	inline constexpr std::uint32_t top12_bits_of_double(double x) noexcept
	{
		return bit_cast<std::uint64_t>(x) >> 52;
	}

	inline constexpr std::uint64_t double_to_uint64(double x) noexcept
	{
		return bit_cast<std::uint64_t>(x);
	}

	inline constexpr double uint64_to_double(std::uint64_t x) noexcept
	{
		return bit_cast<double>(x);
	}

	inline constexpr std::uint32_t float_to_uint32(float x) noexcept
	{
		return bit_cast<std::uint32_t>(x);
	}

	inline constexpr double uint32_to_float(std::uint32_t x) noexcept
	{
		return bit_cast<float>(x);
	}


}
