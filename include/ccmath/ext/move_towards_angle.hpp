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
#include "ccmath/ext/move_towards.hpp"

#include <type_traits>

namespace ccm::ext
{
	/**
	 * @brief Move an angle toward a target angle without overshooting.
	 * @tparam T Type of the input and output.
	 * @param current The current angle, in degrees.
	 * @param target The target angle, in degrees.
	 * @param max_delta The maximum angular change to apply.
	 * @return The moved angle, in degrees.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] constexpr T move_towards_angle(T current, T target, T max_delta) noexcept
	{
		const T delta = ext::delta_angle(current, target);
		return ext::move_towards(current, current + delta, max_delta);
	}

	namespace safe
	{
		/**
		 * @brief Safely move an angle toward a target angle without overshooting.
		 * @tparam T Type of the input and output.
		 * @param current The current angle, in degrees.
		 * @param target The target angle, in degrees.
		 * @param max_delta The maximum angular change to apply.
		 * @return The moved angle, in degrees.
		 */
		template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
		[[nodiscard]] constexpr T move_towards_angle(T current, T target, T max_delta) noexcept
		{
			const T delta = ext::safe::delta_angle(current, target);
			return ext::move_towards(current, current + delta, max_delta);
		}
	} // namespace safe
} // namespace ccm::ext