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

#include "ccmath/ext/repeat.hpp"
#include "ccmath/math/numbers.hpp"

#include <type_traits>

namespace ccm::ext
{
	/**
	 * @brief Compute the shortest signed difference between two angles in radians.
	 * @tparam T Type of the input and output.
	 * @param current The current angle, in radians.
	 * @param target The target angle, in radians.
	 * @return The shortest signed angular difference in the range [-pi, pi].
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T delta_angle(T current, T target) noexcept
	{
		const T two_pi = T(2) * ccm::numbers::pi_v<T>;

		T delta = ext::repeat(target - current, two_pi);

		if (delta > ccm::numbers::pi_v<T>) { delta -= two_pi; }

		return delta;
	}
} // namespace ccm::ext
