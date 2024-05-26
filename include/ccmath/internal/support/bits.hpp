/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

// Support header that brings C++20's <bit> header to C++17.

#pragma once

#include "ccmath/internal/config/type_support.hpp"
#include "ccmath/internal/predef/has_builtin.hpp"
#include "ccmath/internal/support/ctz.hpp"
#include "ccmath/internal/support/type_traits.hpp"

#include <cstdint>

namespace ccm::support
{
	template <typename To, typename From>
	constexpr std::enable_if_t<
		sizeof(To) == sizeof(From) && std::is_trivially_constructible_v<To> && std::is_trivially_copyable_v<To> && std::is_trivially_copyable_v<From>, To>
	bit_cast(const From & from)
	{
		return __builtin_bit_cast(To, from);
	}

	template <class T,
			  std::enable_if_t<traits::ccm_is_integral_v<T> && traits::ccm_is_unsigned_v<T> && !traits::is_char_v<T> && !std::is_same_v<T, bool>, bool> = true>
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

// Macro to allow simplified creation of specializations
// NOLINTBEGIN(bugprone-macro-parentheses)
#define INTERNAL_CCM_ADD_CTZ_SPECIALIZATION(FUNC, TYPE, BUILTIN)                                                                                               \
	template <>                                                                                                                                                \
	[[nodiscard]] constexpr int FUNC<TYPE>(TYPE value)                                                                                                         \
	{                                                                                                                                                          \
		static_assert(ccm::support::traits::ccm_is_unsigned_v<TYPE>);                                                                                          \
		return value == 0 ? std::numeric_limits<TYPE>::digits : BUILTIN(value);                                                                                \
	}
	// NOLINTEND(bugprone-macro-parentheses)

#if CCM_HAS_BUILTIN(__builtin_ctzg)
	/**
	 * @brief Returns the number of consecutive 0 bits in the value of x, starting from the least significant bit ("right").
	 * https://en.cppreference.com/w/cpp/numeric/countr_zero
	 */
	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<ccm::support::traits::ccm_is_unsigned_v<T>, int> countr_zero(T value)
	{
		return __builtin_ctzg(value, std::numeric_limits<T>::digits);
	}
#else  // !CCM_HAS_BUILTIN(__builtin_ctzg)
	/**
	 * @brief Returns the number of consecutive 0 bits in the value of x, starting from the least significant bit ("right").
	 * https://en.cppreference.com/w/cpp/numeric/countr_zero
	 */
	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, int> countr_zero(T value)
	{
		if (value == 0) { return std::numeric_limits<T>::digits; }
		if (value & 0x1) { return 0; }
		// Bisection method
		unsigned zero_bits = 0;
		unsigned shift	   = std::numeric_limits<T>::digits >> 1;
		T mask			   = std::numeric_limits<T>::max() >> shift;
		while (shift)
		{
			if ((value & mask) == 0)
			{
				value >>= shift;
				zero_bits |= shift;
			}
			shift >>= 1;
			mask >>= shift;
		}
		return zero_bits;
	}
#endif // CCM_HAS_BUILTIN(__builtin_ctzg)

#if CCM_HAS_BUILTIN(__builtin_ctzs)
	INTERNAL_CCM_ADD_CTZ_SPECIALIZATION(countr_zero, unsigned short, __builtin_ctzs)
#endif // CCM_HAS_BUILTIN(__builtin_ctzs)
#if CCM_HAS_BUILTIN(__builtin_ctz)
	INTERNAL_CCM_ADD_CTZ_SPECIALIZATION(countr_zero, unsigned int, __builtin_ctz)
#endif // CCM_HAS_BUILTIN(__builtin_ctz)
#if CCM_HAS_BUILTIN(__builtin_ctzl)
	INTERNAL_CCM_ADD_CTZ_SPECIALIZATION(countr_zero, unsigned long, __builtin_ctzl)
#endif // CCM_HAS_BUILTIN(__builtin_ctzl)
#if CCM_HAS_BUILTIN(__builtin_ctzll)
	INTERNAL_CCM_ADD_CTZ_SPECIALIZATION(countr_zero, unsigned long long, __builtin_ctzll)
#endif // CCM_HAS_BUILTIN(__builtin_ctzll)

#if CCM_HAS_BUILTIN(__builtin_clzg)
	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, int> countl_zero(T value)
	{
		return __builtin_clzg(value, std::numeric_limits<T>::digits);
	}
#else  // !CCM_HAS_BUILTIN(__builtin_clzg)
	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, int> countl_zero(T value)
	{
		if (value == 0) { return std::numeric_limits<T>::digits; }
		// Bisection method
		unsigned zero_bits = 0;
		for (unsigned shift = std::numeric_limits<T>::digits >> 1; shift; shift >>= 1)
		{
			T tmp = value >> shift;
			if (tmp) { value = tmp; }
			else { zero_bits |= shift; }
		}
		return zero_bits;
	}
#endif // CCM_HAS_BUILTIN(__builtin_clzg)

#if CCM_HAS_BUILTIN(__builtin_clzs)
	INTERNAL_CCM_ADD_CTZ_SPECIALIZATION(countl_zero, unsigned short, __builtin_clzs)
#endif // CCM_HAS_BUILTIN(__builtin_clzs)
#if CCM_HAS_BUILTIN(__builtin_clz)
	INTERNAL_CCM_ADD_CTZ_SPECIALIZATION(countl_zero, unsigned int, __builtin_clz)
#endif // CCM_HAS_BUILTIN(__builtin_clz)
#if CCM_HAS_BUILTIN(__builtin_clzl)
	INTERNAL_CCM_ADD_CTZ_SPECIALIZATION(countl_zero, unsigned long, __builtin_clzl)
#endif // CCM_HAS_BUILTIN(__builtin_clzl)
#if CCM_HAS_BUILTIN(__builtin_clzll)
	INTERNAL_CCM_ADD_CTZ_SPECIALIZATION(countl_zero, unsigned long long, __builtin_clzll)
#endif // CCM_HAS_BUILTIN(__builtin_clzll)

#undef INTERNAL_CCM_ADD_CTZ_SPECIALIZATION

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, int> countr_one(T value)
	{
		return support::countr_zero<T>(~value);
	}

	template <typename T, std::enable_if_t<traits::is_unsigned_integer_v<T>, bool> = true>
	[[nodiscard]] constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, int> countl_one(T value)
	{
		return value != std::numeric_limits<T>::max() ? countl_zero(static_cast<T>(~value)) : std::numeric_limits<T>::digits;
	}

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, int> bit_width(T value)
	{
		return std::numeric_limits<T>::digits - countl_zero(value);
	}

#if CCM_HAS_BUILTIN(__builtin_popcountg)
	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<std::is_unsigned_v<T>, int> popcount(T value)
	{
		return __builtin_popcountg(value);
	}
#else  // !CCM_HAS_BUILTIN(__builtin_popcountg)
	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, int> popcount(T value)
	{
		int count = 0;
		for (int i = 0; i != std::numeric_limits<T>::digits; ++i)
		{
			if ((value >> i) & 0x1) { ++count; }
		}
		return count;
	}
#endif // CCM_HAS_BUILTIN(__builtin_popcountg)

// Macro to allow simplified creation of specializations
// NOLINTBEGIN(bugprone-macro-parentheses)
#define INTERNAL_CCM_ADD_POPCOUNT_SPECIALIZATION(FUNC, TYPE, BUILTIN)                                                                                          \
	template <>                                                                                                                                                \
	[[nodiscard]] constexpr int FUNC<TYPE>(TYPE value)                                                                                                         \
	{                                                                                                                                                          \
		static_assert(ccm::support::traits::ccm_is_unsigned_v<TYPE>);                                                                                          \
		return BUILTIN(value);                                                                                                                                 \
	}
// NOLINTEND(bugprone-macro-parentheses)
// If the compiler has builtin's for popcount, the create specializations that use the builtin.
#if CCM_HAS_BUILTIN(__builtin_popcount)
	INTERNAL_CCM_ADD_POPCOUNT_SPECIALIZATION(popcount, unsigned char, __builtin_popcount)
	INTERNAL_CCM_ADD_POPCOUNT_SPECIALIZATION(popcount, unsigned short, __builtin_popcount)
	INTERNAL_CCM_ADD_POPCOUNT_SPECIALIZATION(popcount, unsigned, __builtin_popcount)
#endif // CCM_HAS_BUILTIN(__builtin_popcount)

#if CCM_HAS_BUILTIN(__builtin_popcountl)
	INTERNAL_CCM_ADD_POPCOUNT_SPECIALIZATION(popcount, unsigned long, __builtin_popcountl)
#endif // CCM_HAS_BUILTIN(__builtin_popcountl)

#if CCM_HAS_BUILTIN(__builtin_popcountll)
	INTERNAL_CCM_ADD_POPCOUNT_SPECIALIZATION(popcount, unsigned long long, __builtin_popcountll)
#endif // CCM_HAS_BUILTIN(__builtin_popcountll)

#undef INTERNAL_CCM_ADD_POPCOUNT_SPECIALIZATION

} // namespace ccm::support
