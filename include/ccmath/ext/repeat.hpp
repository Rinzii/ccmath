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

#include "ccmath/math/nearest/floor.hpp"

#include <type_traits>

namespace ccm::ext
{
	namespace unsafe
	{
		/**
		 * @brief Repeat a value within the range [0, length) without guarding the length.
		 *
		 * This is the raw form with no zero check. If length is zero the result
		 * follows normal floating-point division semantics and may be NaN.
		 *
		 * @tparam T Type of the input and output.
		 * @param value The value to repeat.
		 * @param length The length of the repeating interval.
		 * @return The repeated value in the range [0, length).
		 */
		template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true> constexpr T repeat(T value, T length) noexcept
		{
			return value - (ccm::floor(value / length) * length);
		}
	} // namespace unsafe

	/**
	 * @brief Repeat a value within the range [0, length).
	 *
	 * If length is zero, this function returns 0 to avoid division by zero and
	 * NaN propagation. For the unguarded form see ccm::ext::unsafe::repeat.
	 *
	 * @tparam T Type of the input and output.
	 * @param value The value to repeat.
	 * @param length The length of the repeating interval.
	 * @return The repeated value in the range [0, length).
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true> constexpr T repeat(T value, T length) noexcept
	{
		if (length == T(0))
		{
			return T(0);
		}

		return ext::unsafe::repeat(value, length);
	}
} // namespace ccm::ext
