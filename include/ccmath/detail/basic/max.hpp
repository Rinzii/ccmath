/*
* Copyright (c) 2024-Present Ian Pike
* Copyright (c) 2024-Present cmath contributors
*
* This library is provided under the MIT License.
* See LICENSE for more information.
*/

#pragma once

// Prevent possible conflicts such as when windows.h is included
#ifdef max
#undef max
#endif

#include <type_traits>
#include <limits>

#include "ccmath/detail/compare/isnan.hpp"

// TODO: Implement a version of fmax that allows two different types to be compared.

namespace ccm
{
	/**
	 * @brief Returns the larger of the two values.
	 * @tparam T Type of the values to compare.
	 * @param a Left-hand side of the comparison.
	 * @param b Right-hand side of the comparison.
	 * @return
	 */
	template <typename T>
    inline constexpr T max(T x, T y)
    {
		if constexpr (std::is_floating_point<T>::value)
		{
			if (ccm::isnan(x) && ccm::isnan(y))
            {
                return std::numeric_limits<T>::quiet_NaN();
            }

            if (ccm::isnan(x))
            {
                return y;
            }

			if (ccm::isnan(y))
            {
                return x;
            }
		}

        return (x > y) ? x : y;
    }

	template <typename T>
	inline constexpr T fmax(T a, T b)
	{
		return max<T>(a, b);
	}

	template <typename Integer, std::enable_if_t<std::is_integral<Integer>::value, int> = 0>
	inline constexpr double fmax(Integer a, Integer b)
	{
		return max<double>(static_cast<double>(a), static_cast<double>(b));
	}

	inline constexpr float fmaxf(float a, float b)
    {
        return fmax<float>(a, b);
    }

	inline constexpr long double fmaxl(long double a, long double b)
    {
        return fmax<long double>(a, b);
    }


} // namespace ccm
