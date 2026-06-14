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
	 * @brief Divide two integral values and round the result toward positive infinity.
	 *
	 * This function is intended for non-negative integral values.
	 *
	 * @tparam T Type of the input and output.
	 * @param value The dividend.
	 * @param divisor The divisor.
	 * @return The ceiling of value divided by divisor.
	 */
	template <typename T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<std::remove_cv_t<T>, bool>, bool> = true>
	constexpr T ceil_div(T value, T divisor) noexcept
	{ return static_cast<T>((value + divisor - T(1)) / divisor); }

	namespace safe
	{
		/**
		 * @brief Safely divide two integral values and round the result toward positive infinity.
		 *
		 * If divisor is zero, this function returns 0.
		 *
		 * This function is intended for non-negative integral values.
		 *
		 * @tparam T Type of the input and output.
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