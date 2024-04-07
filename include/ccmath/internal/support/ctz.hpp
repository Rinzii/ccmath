/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/predef/has_builtin.hpp"

#include <limits>
#include <type_traits>

namespace ccm::support
{
	namespace internal
	{
		// Software implementation of ctz for unsigned integral types in the event that the compiler does not provide a builtin
		// Mostly added for msvc support, as gcc and clang have builtins for this.
		template <typename T, std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T> && !std::is_same_v<T, bool>, bool> = true>
		constexpr int generic_ctz(T x) noexcept
		{
			// If x is 0, the result is undefined.
			if (x == 0)
			{
				// We return the size of the type in bits to indicate undefined behavior.
				// This mimics the behavior of __builtin_ctz.
				return sizeof(T) * std::numeric_limits<unsigned char>::digits;
			}

			int count = 0;

			// Loop until the least significant bit is 1
			while ((x & 1) == 0)
			{
				++count;
				x >>= 1; // Right shift x by 1 bit
			}

			return count;
		}
	} // namespace internal

	template <typename T>
	constexpr int ctz(T /* x */) noexcept;

	template <>
	constexpr int ctz(unsigned short x) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_ctzs)
		return __builtin_ctzs(x);
#else
		return internal::generic_ctz(x);
#endif
	}

	template <>
	constexpr int ctz(unsigned int x) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_ctz)
		return __builtin_ctz(x);
#else
		return internal::generic_ctz(x);
#endif
	}

	template <>
	constexpr int ctz(unsigned long x) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_ctzl)
		return __builtin_ctzl(x);
#else
		return internal::generic_ctz(x);
#endif
	}

	template <>
	constexpr int ctz(unsigned long long x) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_ctzll)
		return __builtin_ctzll(x);
#else
		return internal::generic_ctz(x);
#endif
	}
} // namespace ccm::support
