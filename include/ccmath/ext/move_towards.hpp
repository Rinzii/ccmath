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
	 * @brief Move a value toward a target without overshooting.
	 * @tparam T Type of the input and output.
	 * @param current The current value.
	 * @param target The target value.
	 * @param max_delta The maximum change to apply.
	 * @return The moved value.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] constexpr T move_towards(T current, T target, T max_delta) noexcept
	{
		const T delta = target - current;

		if (ccm::abs(delta) <= max_delta) { return target; }

		return current + ((delta > T(0)) ? max_delta : -max_delta);
	}
} // namespace ccm::ext