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

#include "ccmath/math/basic/fabs.hpp"

#include <type_traits>

namespace ccm::ext
{
	/**
	 * @brief Check whether two floating-point values are approximately equal.
	 * @tparam T Type of the input values.
	 * @param lhs The first value.
	 * @param rhs The second value.
	 * @param epsilon The maximum allowed absolute difference.
	 * @return True if the absolute difference between lhs and rhs is less than or equal to epsilon.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] constexpr bool approximately(T lhs, T rhs, T epsilon) noexcept
	{
		return ccm::abs(lhs - rhs) <= epsilon;
	}
} // namespace ccm::ext