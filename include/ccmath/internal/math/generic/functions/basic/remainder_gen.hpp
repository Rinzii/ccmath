/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/math/nearest/trunc.hpp"

namespace ccm
{
	/**
	 * @brief Returns the remainder of the division of x by y.
	 * @tparam T Type of the values to compare.
	 * @param x Dividend.
	 * @param y Divisor.
	 * @return The remainder of the division of x by y.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T remainder(T x, T y)
	{
		using FPBits_t = typename ccm::support::fp::FPBits<T>;
		const FPBits_t x_bits(x);
		const FPBits_t y_bits(y);

		const bool x_is_nan = x_bits.is_nan();
		const bool y_is_nan = y_bits.is_nan();

		// If x is ±∞ and y is not NaN, NaN is returned.
		// If y is ±0 and x is not NaN, NaN is returned.
		// If either argument is NaN, NaN is returned.
		if (CCM_UNLIKELY((x_bits.is_inf() && !y_is_nan) || (y_bits.is_zero() && !x_is_nan) || (x_is_nan || y_is_nan)))
		{
			// All major compilers return -NaN.
			return -std::numeric_limits<T>::quiet_NaN();
		}

		return static_cast<T>(x - (ccm::trunc<T>(x / y) * y));
	}

	/**
	 * @brief Returns the remainder of the division of x by y.
	 * @tparam Integer Type of the values to compare.
	 * @param x Dividend.
	 * @param y Divisor.
	 * @return The remainder of the division of x by y as a double.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double remainder(Integer x, Integer y)
	{
		return ccm::remainder<double>(static_cast<double>(x), static_cast<double>(y));
	}

	/**
	 * @brief Returns the remainder of the division of x by y.
	 * @param x Dividend.
	 * @param y Divisor.
	 * @return The remainder of the division of x by y.
	 */
	constexpr float remainderf(float x, float y)
	{
		return ccm::remainder<float>(x, y);
	}

	/**
	 * @brief Returns the remainder of the division of x by y.
	 * @param x Dividend.
	 * @param y Divisor.
	 * @return The remainder of the division of x by y.
	 */
	constexpr long double remainderl(long double x, long double y)
	{
		return ccm::remainder<long double>(x, y);
	}
} // namespace ccm

/// @ingroup basic
