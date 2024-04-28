/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <type_traits>

namespace ccm::ext
{
	/**
	 * @brief Cubic interpolation.
	 * @tparam T Type of the input and output.
	 * @param y0 The first value.
	 * @param y1 The second value.
	 * @param y2 The third value.
	 * @param y3 The fourth value.
	 * @param t The interpolation value.
	 * @return The interpolated value.
	 */
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T cubic(T y0, T y1, T y2, T y3, T t)
	{
		const T a0 = y3 - y2 - y0 + y1;
		const T a1 = y0 - y1 - a0;
		const T a2 = y2 - y0;
		const T a3 = y1;

		const T t2 = t * t;

		return a0 * t * t2 + a1 * t2 + a2 * t + a3;
	}
} // namespace ccm::ext