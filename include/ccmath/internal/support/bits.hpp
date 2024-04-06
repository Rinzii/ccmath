/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

// Support header that brings C++20's <bit> header to C++17.

#pragma once

#include "ccmath/internal/support/ctz.hpp"
#include "ccmath/internal/predef/has_builtin.hpp"

#include <cstdint>
#include <type_traits>

namespace ccm::support::traits
{
	// TODO: Later add this into its own header.
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
	template <typename T> constexpr bool is_char_v = is_char<T>::value;

    template <typename T> struct is_unsigned_integer : std::false_type {};
    template <> struct is_unsigned_integer<unsigned char> : std::true_type {};
    template <> struct is_unsigned_integer<unsigned short> : std::true_type {};
    template <> struct is_unsigned_integer<unsigned int> : std::true_type {};
    template <> struct is_unsigned_integer<unsigned long> : std::true_type {};
    template <> struct is_unsigned_integer<unsigned long long> : std::true_type {};
#if defined(__SIZEOF_INT128__)
    template <> struct is_unsigned_integer<__uint128_t> : std::true_type {};
#endif
    template <typename T> constexpr bool is_unsigned_integer_v = is_unsigned_integer<T>::value;
	// clang-format on

} // namespace ccm::support::traits

namespace ccm::support
{

	/**
	 * @brief
	 * @tparam To
	 * @tparam From
	 * @param src
	 * @return
	 */
	template <class To, class From>
	std::enable_if_t<sizeof(To) == sizeof(From) && std::is_trivially_copyable_v<From> && std::is_trivially_copyable_v<To>, To> constexpr bit_cast(
		const From & src) noexcept
	{
		static_assert(std::is_trivially_constructible_v<To>, "This implementation additionally requires "
															 "destination type to be trivially constructible");

		return __builtin_bit_cast(To, src);
	}

	template <class T,
			  std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T> && !ccm::support::traits::is_char_v<T> && !std::is_same_v<T, bool>, bool> = true>
	constexpr bool has_single_bit(T x) noexcept
	{
		return x && !(x & (x - 1));
	}

	/**
	 * @brief Helper function to get the top 16-bits of a double.
	 * @param x Double to get the bits from.
	 * @return
	 */
	constexpr std::uint32_t top16_bits_of_double(double x) noexcept
	{
		return static_cast<std::uint32_t>(bit_cast<std::uint64_t>(x) >> 48);
	}

	constexpr std::uint32_t top12_bits_of_double(double x) noexcept
	{
		return static_cast<std::uint32_t>(bit_cast<std::uint64_t>(x) >> 52);
	}

	constexpr std::uint32_t top12_bits_of_float(float x) noexcept
	{
		return bit_cast<std::uint32_t>(x) >> 20;
	}

	constexpr std::uint64_t double_to_uint64(double x) noexcept
	{
		return bit_cast<std::uint64_t>(x);
	}

	constexpr std::int64_t double_to_int64(double x) noexcept
	{
		return bit_cast<std::int64_t>(x);
	}

	constexpr double uint64_to_double(std::uint64_t x) noexcept
	{
		return bit_cast<double>(x);
	}

	constexpr double int64_to_double(std::int64_t x) noexcept
	{
		return bit_cast<double>(x);
	}

	constexpr std::uint32_t float_to_uint32(float x) noexcept
	{
		return bit_cast<std::uint32_t>(x);
	}

	constexpr std::int32_t float_to_int32(float x) noexcept
	{
		return bit_cast<std::int32_t>(x);
	}

	constexpr float uint32_to_float(std::uint32_t x) noexcept
	{
		return bit_cast<float>(x);
	}

	constexpr float int32_to_float(std::int32_t x) noexcept
	{
		return bit_cast<float>(x);
	}

	/**
	 * @brief Rotates unsigned integer bits to the right.
	 * https://en.cppreference.com/w/cpp/numeric/rotr
	 */
	template <class T>
	constexpr T rotr(T t, int cnt) noexcept
	{
		static_assert(ccm::support::traits::is_unsigned_integer_v<T>, "rotr requires an unsigned integer type");
		const unsigned int dig = std::numeric_limits<T>::digits;
		if ((static_cast<unsigned int>(cnt) % dig) == 0) { return t; }

		if (cnt < 0)
		{
			cnt *= -1;
			return (t << (static_cast<unsigned int>(cnt) % dig)) |
				   (t >> (dig - (static_cast<unsigned int>(cnt) % dig))); // rotr with negative cnt is similar to rotl
		}

		return (t >> (static_cast<unsigned int>(cnt) % dig)) | (t << (dig - (static_cast<unsigned int>(cnt) % dig)));
	}

	/**
	 * @brief Rotates unsigned integer bits to the left.
	 * https://en.cppreference.com/w/cpp/numeric/rotl
	 */
	template <class T>
	constexpr T rotl(T t, int cnt) noexcept
	{
		return rotr(t, -cnt);
	}

	// https://en.cppreference.com/w/cpp/numeric/countr_zero
	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<std::is_unsigned_v<T>, int> countr_zero(T value)
	{
		if (value == 0) { return std::numeric_limits<T>::digits; }

		if constexpr (ccm::support::traits::is_unsigned_integer_v<T>) { return ccm::support::ctz<T>(value); }

		int ret						 = 0;
		const unsigned int ulldigits = std::numeric_limits<unsigned long long>::digits;
		while (static_cast<unsigned long long>(value) == 0ULL)
		{
			ret += ulldigits;
			value >>= ulldigits;
		}
		return ret + ccm::support::ctz(static_cast<unsigned long long>(value));
	}

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<std::is_unsigned_v<T>, int> countr_one(T value)
	{
		return value != std::numeric_limits<T>::max() ? countr_zero(static_cast<T>(~value)) : std::numeric_limits<T>::digits;
	}

	template <typename T, std::enable_if_t<ccm::support::traits::is_unsigned_integer_v<T>, bool> = true>
	[[nodiscard]] constexpr std::enable_if_t<std::is_unsigned_v<T>, int> countl_zero(T value)
	{
		if (value == 0) { return std::numeric_limits<T>::digits; }

		if constexpr (ccm::support::traits::is_unsigned_integer_v<T>) { return std::numeric_limits<T>::digits - ccm::support::ctz<T>(value); }

		int ret						 = 0;
		int iter					 = 0;
		const unsigned int ulldigits = std::numeric_limits<unsigned long long>::digits;
		while (true)
		{
			value = rotl(value, ulldigits);
			if ((iter = countl_zero(static_cast<unsigned long long>(value))) != ulldigits) // NOLINT
				break;
			ret += iter;
		}
		return ret + iter;
	}

	template <typename T, std::enable_if_t<ccm::support::traits::is_unsigned_integer_v<T>, bool> = true>
	[[nodiscard]] constexpr std::enable_if_t<std::is_unsigned_v<T>, int> countl_one(T value)
	{
		return value != std::numeric_limits<T>::max() ? countl_zero(static_cast<T>(~value)) : std::numeric_limits<T>::digits;
	}

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<std::is_unsigned_v<T>, int> bit_width(T value)
	{
		return std::numeric_limits<T>::digits - countl_zero(value);
	}

#if CCM_HAS_BUILTIN(__builtin_popcountg)
	template <typename T>
	[[nodiscard]] LIBC_INLINE constexpr cpp::enable_if_t<cpp::is_unsigned_v<T>, int> popcount(T value)
	{
		return __builtin_popcountg(value);
	}
#else  // !CCM_HAS_BUILTIN(__builtin_popcountg)
	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<std::is_unsigned_v<T>, int> popcount(T value)
	{
		int count = 0;
		for (int i = 0; i != std::numeric_limits<T>::digits; ++i)
		{
			if ((value >> i) & 0x1) { ++count; }
		}
		return count;
	}
#endif // CCM_HAS_BUILTIN(__builtin_popcountg)

// If the compiler has builtin's for popcount, the create specializations that use the builtin.
#if CCM_HAS_BUILTIN(__builtin_popcount)
	template <>
	[[nodiscard]] constexpr int popcount<unsigned char>(unsigned char value)
	{
		return __builtin_popcount(value);
	}

	template <>
	[[nodiscard]] constexpr int popcount<unsigned short>(unsigned short value)
	{
		return __builtin_popcount(value);
	}

	template <>
	[[nodiscard]] constexpr int popcount<unsigned>(unsigned value)
	{
		return __builtin_popcount(value);
	}
#endif // CCM_HAS_BUILTIN(__builtin_popcount)

#if CCM_HAS_BUILTIN(__builtin_popcountl)
	template <>
	[[nodiscard]] constexpr int popcount<unsigned long>(unsigned long value)
	{
		return __builtin_popcountl(value);
	}
#endif // CCM_HAS_BUILTIN(__builtin_popcountl)

#if CCM_HAS_BUILTIN(__builtin_popcountll)
	template <>
	[[nodiscard]] constexpr int popcount<unsigned long long>(unsigned long long value)
	{
		return __builtin_popcountll(value);
	}
#endif // CCM_HAS_BUILTIN(__builtin_popcountll)

} // namespace ccm::support
