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

#include <type_traits>

namespace ccm::ext
{
	/**
	 * @brief Compute the shortest signed difference between two angles in degrees.
	 * @tparam T Type of the input and output.
	 * @param current The current angle, in degrees.
	 * @param target The target angle, in degrees.
	 * @return The shortest signed angular difference in the range [-180, 180].
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] constexpr T delta_angle(T current, T target) noexcept
	{
		T delta = ext::repeat(target - current, T(360));

		if (delta > T(180)) {
			delta -= T(360);
		}

		return delta;
	}

	namespace safe
	{
		/**
		 * @brief Safely compute the shortest signed difference between two angles in degrees.
		 * @tparam T Type of the input and output.
		 * @param current The current angle, in degrees.
		 * @param target The target angle, in degrees.
		 * @return The shortest signed angular difference in the range [-180, 180].
		 */
		template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
		[[nodiscard]] constexpr T delta_angle(T current, T target) noexcept
		{
			T delta = ext::safe::repeat(target - current, T(360));

			if (delta > T(180)) {
				delta -= T(360);
			}

			return delta;
		}
	} // namespace safe
} // namespace ccm::ext
