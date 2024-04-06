/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <type_traits>

namespace ccm
{
	template <typename T>
	struct NumberPair
	{
		T hi;
		T lo;
	};

	template <typename T>
	std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>, NumberPair<T>> split(T x)
	{
		constexpr std::size_t half_bits_width = sizeof(T) * 4;
		constexpr T Low_mask				  = (T{1} << half_bits_width) - T{1};
		NumberPair<T> result;
		result.lo = x & Low_mask;
		result.hi = x >> half_bits_width;
		return result;
	}
} // namespace ccm
