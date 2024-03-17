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

namespace ccm::helpers::traits
{
	// clang-format off
	template <typename T> struct is_char : std::false_type {};
	template <> struct is_char<char> : std::true_type {};
	template <> struct is_char<wchar_t> : std::true_type {};
#if (__cplusplus >= 202002L) || defined(__cpp_char8_t) || defined(__cpp_lib_char8_t) // C++20 defines char8_t
    template <> struct is_char<char8_t> : std::true_type {};
#endif
	template <> struct is_char<char16_t> : std::true_type {};
	template <> struct is_char<char32_t> : std::true_type {};
	template <> struct is_char<signed char> : std::true_type {};
	template <> struct is_char<unsigned char> : std::true_type {};
	template <typename T> inline constexpr bool is_char_v = is_char<T>::value;
	// clang-format on

} // namespace ccm::helpers::traits

namespace ccm::helpers
{

	/**
	 * @brief
	 * @tparam To
	 * @tparam From
	 * @param src
	 * @return
	 */
	template <class To, class From>
	std::enable_if_t<sizeof(To) == sizeof(From) && std::is_trivially_copyable_v<From> && std::is_trivially_copyable_v<To>, To> inline constexpr bit_cast(
		const From & src) noexcept
	{
		static_assert(std::is_trivially_constructible_v<To>, "This implementation additionally requires "
															 "destination type to be trivially constructible");

		return __builtin_bit_cast(To, src);
	}

	template <class T, std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T> && !ccm::helpers::traits::is_char_v<T> && !std::is_same_v<T, bool>,
										bool> = true>
	constexpr bool has_single_bit(T x) noexcept
	{
		return x && !(x & (x - 1));
	}




	/**
	 * @brief Helper function to get the top 16-bits of a double.
	 * @param x Double to get the bits from.
	 * @return
	 */
	inline constexpr std::uint32_t top16_bits_of_double(double x) noexcept
	{
		return static_cast<std::uint32_t>(bit_cast<std::uint64_t>(x) >> 48);
	}

	inline constexpr std::uint32_t top12_bits_of_double(double x) noexcept
	{
		return static_cast<std::uint32_t>(bit_cast<std::uint64_t>(x) >> 52);
	}

	inline constexpr std::uint64_t double_to_uint64(double x) noexcept
	{
		return bit_cast<std::uint64_t>(x);
	}

	inline constexpr std::int64_t double_to_int64(double x) noexcept
	{
		return bit_cast<std::int64_t>(x);
	}

	inline constexpr double uint64_to_double(std::uint64_t x) noexcept
	{
		return bit_cast<double>(x);
	}

	inline constexpr double int64_to_double(std::int64_t x) noexcept
    {
        return bit_cast<double>(x);
    }

	inline constexpr std::uint32_t float_to_uint32(float x) noexcept
	{
		return bit_cast<std::uint32_t>(x);
	}

	inline constexpr std::int32_t float_to_int32(float x) noexcept
    {
        return bit_cast<std::int32_t>(x);
    }

	inline constexpr float uint32_to_float(std::uint32_t x) noexcept
	{
		return bit_cast<float>(x);
	}

	inline constexpr float int32_to_float(std::int32_t x) noexcept
    {
        return bit_cast<float>(x);
    }


} // namespace ccm::helpers
