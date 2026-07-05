/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#pragma once

#include "ccmath/internal/math/runtime/func/basic/remainder_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/basic/remquo.hpp"

namespace ccm
{
	/**
	 * @brief Returns the remainder of the division of x by y.
	 * @tparam T Type of the values to compare.
	 * @param x Dividend.
	 * @param y Divisor.
	 * @return The remainder of the division of x by y.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true> constexpr T remainder(T x, T y)
	{
		if (!ccm::support::is_constant_evaluated())
		{
			return ccm::rt::remainder_rt(x, y);
		}

		// remainder is the remainder component of remquo, so reuse the exact iterative reduction
		// already implemented there. The quotient rounds to nearest, ties-to-even, which keeps the
		// result in the closed interval from -abs(y) / 2 to +abs(y) / 2. The result is an exact
		// subtraction, so it does not depend on the active rounding mode, and the remquo kernel also
		// covers the special cases (remainder of a finite value by an infinity is that finite value,
		// and NaN is returned for an infinite dividend, a zero divisor, or a NaN argument).
		int quotient = 0;
		return ccm::remquo<T>(x, y, &quotient);
	}

	/**
	 * @brief Returns the remainder of the division of x by y.
	 * @tparam Integer Type of the values to compare.
	 * @param x Dividend.
	 * @param y Divisor.
	 * @return The remainder of the division of x by y as a double.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true> constexpr double remainder(Integer x, Integer y)
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
