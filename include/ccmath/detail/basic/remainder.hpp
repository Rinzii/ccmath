/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/detail/compare/isinf.hpp"
#include "ccmath/detail/compare/isnan.hpp"
#include "ccmath/detail/nearest/trunc.hpp"

namespace ccm
{

	/**
	 * @brief Returns the remainder of the division of x by y.
	 * @tparam T Type of the values to compare.
	 * @param x Dividend.
	 * @param y Divisor.
	 * @return
	 */
	template <typename T, std::enable_if_t<!std::is_integral<T>::value, int> = 0>
	inline constexpr T remainder(T x, T y)
	{
		if constexpr (std::is_floating_point_v<T>)
		{
			// If x is ±∞ and y is not NaN, NaN is returned.
			// If y is ±0 and x is not NaN, NaN is returned.
			// If either argument is NaN, NaN is returned.
			if ((ccm::isinf(x) && !ccm::isnan(y)) || (y == 0 && !ccm::isnan(x)) || (ccm::isnan(x) || ccm::isnan(y)))
			{
				return std::numeric_limits<T>::quiet_NaN();
			}
		}

		return static_cast<T>(x - (ccm::trunc<T>(x / y) * y));
	}

	template <typename Integer, std::enable_if_t<std::is_integral<Integer>::value, int> = 0>
	inline constexpr double remainder(Integer x, Integer y)
	{
		return remainder<double>(static_cast<double>(x), static_cast<double>(y));
	}

} // namespace ccm
