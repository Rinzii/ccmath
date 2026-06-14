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

#include <type_traits>

namespace ccm::ext
{
	/**
	 * @brief Divide two integral values and round the quotient toward positive infinity.
	 *
	 * Works for signed and unsigned types and rounds correctly for negative operands.
	 * The behavior is undefined if divisor is zero or if the exact quotient is not
	 * representable in T, for example the most negative value divided by minus one.
	 *
	 * @tparam T Integral type of the inputs and output.
	 * @param value The dividend.
	 * @param divisor The divisor.
	 * @return The ceiling of value divided by divisor.
	 */
	template <typename T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<std::remove_cv_t<T>, bool>, bool> = true>
	constexpr T ceil_div(T value, T divisor) noexcept
	{
		// Divide first, then add the correction, so there is no pre-division addition that can overflow.
		if constexpr (std::is_signed_v<T>)
		{
			const bool quotient_positive = (value < 0) == (divisor < 0);
			return static_cast<T>(value / divisor + (value % divisor != 0 && quotient_positive));
		}
		else
		{
			return static_cast<T>(value / divisor + (value % divisor != 0));
		}
	}

	namespace safe
	{
		/**
		 * @brief Safely divide two integral values and round the quotient toward positive infinity.
		 *
		 * If divisor is zero, this function returns 0. Otherwise it matches ext::ceil_div,
		 * which works for signed and unsigned types and rounds correctly for negative operands.
		 *
		 * @tparam T Integral type of the inputs and output.
		 * @param value The dividend.
		 * @param divisor The divisor.
		 * @return The ceiling of value divided by divisor.
		 */
		template <typename T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<std::remove_cv_t<T>, bool>, bool> = true>
		constexpr T ceil_div(T value, T divisor) noexcept
		{
			if (divisor == T(0)) { return T(0); }

			return ext::ceil_div(value, divisor);
		}
	} // namespace safe
} // namespace ccm::ext