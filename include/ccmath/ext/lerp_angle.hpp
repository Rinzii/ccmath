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

#include "ccmath/ext/delta_angle.hpp"

#include <type_traits>

namespace ccm::ext
{
	/**
	 * @brief Linearly interpolate between two angles in radians along the shortest arc.
	 *
	 * This function does not clamp t. Values of t outside [0, 1] extrapolate.
	 *
	 * @tparam T Type of the input and output.
	 * @param start The start angle, in radians.
	 * @param end The end angle, in radians.
	 * @param t The interpolation parameter.
	 * @return The interpolated angle, in radians.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true> constexpr T lerp_angle(T start, T end, T t) noexcept
	{
		return start + (ext::delta_angle(start, end) * t);
	}
} // namespace ccm::ext
