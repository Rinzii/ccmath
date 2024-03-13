/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/detail/basic/impl/remquo_double_impl.hpp"
#include "ccmath/detail/basic/impl/remquo_float_impl.hpp"

namespace ccm
{
	/**
	 * @brief  Signed remainder as well as the three last bits of the division operation
	 * @tparam T The type of the arguments
	 * @param x Floating-point or integer values
	 * @param y Floating-point or integer values
	 * @param quo Pointer to int to store the sign and some bits of x / y
	 * @return If successful, returns the floating-point remainder of the division x / y as defined in ccm::remainder, and stores, in *quo, the sign and at
	 * least three of the least significant bits of x / y
	 *
	 * @attention If you want the quotient pointer to work within a constant context you must perform something like as follows: (The below code will work with
	 * constexpr and static_assert)
	 *
	 * @code
	 * constexpr double get_remainder(double x, double y)
	 * {
	 *      int quotient {0};
	 *      double remainder = ccm::remquo(x, y, &quotient);
	 *      return remainder;
	 *  }
	 *
	 *  constexpr int get_quotient(double x, double y)
	 *  {
	 *      int quotient {0};
	 *      ccm::remquo(x, y, &quotient);
	 *      return quotient;
	 *  }
	 *  @endcode
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	inline constexpr T remquo(T x, T y, int * quo)
	{
		if constexpr (std::is_same_v<T, float>) { return ccm::internal::remquo_float(x, y, quo); }
		else { return ccm::internal::remquo_double(x, y, quo); }
	}
} // namespace ccm

/// @ingroup basic
