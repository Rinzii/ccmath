/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present cmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <type_traits>

#include "ccmath/math/exponential/exp2.hpp"
#include "ccmath/math/exponential/log2.hpp"

#include "ccmath/math/power/impl/pow_impl.hpp"


namespace ccm
{
	namespace internal::impl
	{
		template <typename T, std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>, bool> = true>
		constexpr T pow_expo_by_sqr(T base, T exp) noexcept
		{
			// Handle common cases
			if (exp == 0) { return 1; }			  // Anything to the power of 0 is 1
			if (exp == 1) { return base; }		  // Anything to the power of 1 is itself
			if (exp == 2) { return base * base; } // Anything to the power of 2 is itself squared
			if (base == 0) { return 0; }		  // 0 to any power is 0
			if (base == 1) { return 1; }		  // 1 to any power is 1

			// If the base is 2, we can use the bit shift operator to calculate the power.
			if (base == 2) { return 1 << exp; }

			// This is pretty fast with smaller numbers, but is slower than the standard when dealing with large numbers.
			// TODO: Find a way to optimize this for larger numbers.
			T result = 1;
			for (;;)
			{
				if (exp & 1) { result *= base; }
				exp >>= 1;
				if (!exp) { break; }
				base *= base;
			}

			return result;
		}

		template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
		constexpr T pow_generic(T base, T exp) noexcept
		{
			// This should work on all x86 platforms but may not work with other architectures.
			// For now this is more of a hold over till I have time to implement a better generic version.
			return ccm::exp2(exp * ccm::log2(base));
		}
	} // namespace internal::impl

	template <typename T>
	constexpr T pow(T base, T exp) noexcept
	{
		if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>) { return internal::impl::pow_expo_by_sqr(base, exp); }
		return internal::impl::pow_generic(base, exp);
	}

} // namespace ccm
