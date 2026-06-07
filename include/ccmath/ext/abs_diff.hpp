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
	 * @brief Compute the absolute difference between two integral values without unsigned underflow.
	 * @tparam T Type of the input and output.
	 * @param lhs The first value.
	 * @param rhs The second value.
	 * @return The absolute difference between lhs and rhs.
	 */
	template <
		typename T,
		std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<std::remove_cv_t<T>, bool>, bool> = true
	>
	[[nodiscard]] constexpr T abs_diff(T lhs, T rhs) noexcept
	{
		return lhs > rhs ? static_cast<T>(lhs - rhs) : static_cast<T>(rhs - lhs);
	}
} // namespace ccm::ext