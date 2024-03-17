/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/basic/fma.hpp"

namespace ccm
{
	template <typename T>
	inline constexpr T lerp2(T a, T b, T t) noexcept
	{
		return fma(t, b, fma(-t, a, a));
	}

	template <typename T>
	inline constexpr T lerp(T a, T b, T t) noexcept
	{
		if ((a <= 0 && b >= 0) || (a >= 0 && b <= 0))
		{
			return t * b + (1 - t) * a;
		}

		if (t == 1)
		{
			return b;
		}

		const T x = a + t * (b - a);
		if ((t > 1) == (b > a))
		{
			return b < x ? x : b;
		}

		return x < b ? x : b;
	}

	template <typename T, typename U, typename V>
	inline constexpr typename std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U> && std::is_arithmetic_v<V>, std::common_type_t<T, U, V> >
	lerp(T a, U b, V t) noexcept
    {
		typedef typename std::common_type_t<T, U, V> result_type;
		static_assert(!(std::is_same_v<T, result_type> && std::is_same_v<U, result_type> && std::is_same_v<V, result_type>));
		return lerp(static_cast<result_type>(a), static_cast<result_type>(b), static_cast<result_type>(t));
    }
} // namespace ccm
