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
	namespace unsafe
	{
		/**
		 * @brief Compute the inverse linear interpolation parameter of a value without guarding the range length.
		 *
		 * This function does not clamp the result. Values outside [start, end]
		 * return values outside [0, 1].
		 *
		 * This is the raw form with no zero check. If start == end the result
		 * follows normal floating-point division semantics and may be NaN.
		 *
		 * @tparam T Type of the input and output.
		 * @param start The start of the source range.
		 * @param end The end of the source range.
		 * @param value The value to evaluate.
		 * @return The interpolation parameter of value in the range [start, end].
		 */
		template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
		constexpr T unlerp(T start, T end, T value) noexcept
		{ return (value - start) / (end - start); }
	} // namespace unsafe

	/**
	 * @brief Compute the inverse linear interpolation parameter of a value.
	 *
	 * This function does not clamp the result. Values outside [start, end]
	 * return values outside [0, 1].
	 *
	 * If start == end, the source range has zero length. In that case this
	 * function returns 0 to avoid division by zero and NaN propagation. For the
	 * unguarded form see ccm::ext::unsafe::unlerp.
	 *
	 * @tparam T Type of the input and output.
	 * @param start The start of the source range.
	 * @param end The end of the source range.
	 * @param value The value to evaluate.
	 * @return The interpolation parameter of value in the range [start, end].
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T unlerp(T start, T end, T value) noexcept
	{
		const T length = end - start;

		if (length == T(0)) { return T(0); }

		return ext::unsafe::unlerp(start, end, value);
	}
} // namespace ccm::ext
