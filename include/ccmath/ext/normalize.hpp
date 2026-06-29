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

#include "ccmath/ext/clamp.hpp"

#include <type_traits>

namespace ccm::ext
{
	namespace unsafe
	{
		/**
		 * @brief Normalize a value to a range without guarding the range length.
		 *
		 * Maps value from the range [min, max] to [0, 1]. This is the raw form
		 * with no zero check. If min == max the result follows normal
		 * floating-point division semantics and may be NaN.
		 *
		 * @tparam T Type of the input and output.
		 * @param value The value to normalize.
		 * @param min The minimum value of the range.
		 * @param max The maximum value of the range.
		 * @return The normalized value.
		 */
		template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true> constexpr T normalize(T value, T min = T(0), T max = T(1)) noexcept
		{
			return ext::clamp((value - min) / (max - min), T(0), T(1));
		}
	} // namespace unsafe

	/**
	 * @brief Normalize a value to a range.
	 *
	 * Maps value from the range [min, max] to [0, 1].
	 *
	 * If min == max, the source range has zero length. In that case this
	 * function returns 0 to avoid division by zero and NaN propagation. For the
	 * unguarded form see ccm::ext::unsafe::normalize.
	 *
	 * @tparam T Type of the input and output.
	 * @param value The value to normalize.
	 * @param min The minimum value of the range.
	 * @param max The maximum value of the range.
	 * @return The normalized value.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true> constexpr T normalize(T value, T min = T(0), T max = T(1)) noexcept
	{
		const T length = max - min;

		if (length == T(0))
		{
			return T(0);
		}

		return ext::unsafe::normalize(value, min, max);
	}
} // namespace ccm::ext
