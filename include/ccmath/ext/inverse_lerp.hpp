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

#include "ccmath/ext/unlerp.hpp"

#include <type_traits>

namespace ccm::ext
{
	/**
	 * @brief Compute the inverse linear interpolation parameter of a value.
	 *
	 * This is an alias-style wrapper around ext::unlerp.
	 *
	 * @tparam T Type of the input and output.
	 * @param start The start of the source range.
	 * @param end The end of the source range.
	 * @param value The value to evaluate.
	 * @return The interpolation parameter of value in the range [start, end].
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T inverse_lerp(T start, T end, T value) noexcept
	{ return ext::unlerp(start, end, value); }

	namespace safe
	{
		/**
		 * @brief Safely compute the inverse linear interpolation parameter of a value.
		 *
		 * This is an alias-style wrapper around ext::safe::unlerp.
		 *
		 * If start == end, the source range has zero length. In that case this
		 * function returns 0 to avoid division by zero and NaN propagation.
		 *
		 * @tparam T Type of the input and output.
		 * @param start The start of the source range.
		 * @param end The end of the source range.
		 * @param value The value to evaluate.
		 * @return The interpolation parameter of value in the range [start, end].
		 */
		template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
		constexpr T inverse_lerp(T start, T end, T value) noexcept
		{ return ext::safe::unlerp(start, end, value); }
	} // namespace safe
} // namespace ccm::ext
