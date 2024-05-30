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

#include "ccmath/internal/config/type_support.hpp"
#include "ccmath/internal/support/type_traits.hpp"
#include "ccmath/internal/types/float128.hpp"

namespace ccm::support
{
	namespace internal
	{
		// Software implementation of ctz for unsigned integral types in the event that the compiler does not provide a builtin
		// Mostly added for msvc support, as gcc and clang have builtins for this.
		template <typename T, std::enable_if_t<traits::ccm_is_integral_v<T> && traits::ccm_is_unsigned_v<T> && !std::is_same_v<T, bool>, bool> = true>
		constexpr int generic_ctz(T value) noexcept
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
			return static_cast<int>(zero_bits);
		}
	} // namespace internal

	template <typename T, std::enable_if_t<traits::ccm_is_integral_v<T> && traits::ccm_is_unsigned_v<T> && !std::is_same_v<T, bool>, bool> = true>
	constexpr int ctz(T value) noexcept
	{
		#if CCM_HAS_BUILTIN(__builtin_ctzg)
			return __builtin_ctzg(value);
		#else
		return internal::generic_ctz(value);
		#endif
	}

	template <>
	constexpr int ctz(unsigned short value) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_ctzs)
		return __builtin_ctzs(value);
#else
		return internal::generic_ctz(value);
#endif
	}

	template <>
	constexpr int ctz(unsigned int value) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_ctz)
		return __builtin_ctz(value);
#else
		return internal::generic_ctz(value);
#endif
	}

	template <>
	constexpr int ctz(unsigned long value) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_ctzl)
		return __builtin_ctzl(value);
#else
		return internal::generic_ctz(value);
#endif
	}

	template <>
	constexpr int ctz(unsigned long long value) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_ctzll)
		return __builtin_ctzll(value);
#else
		return internal::generic_ctz(value);
#endif
	}
} // namespace ccm::support
