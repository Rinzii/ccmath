/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * This library is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#pragma once

#include <type_traits>

namespace ccm::ext
{
	/**
	 * @brief Performs a linear interpolation between x and y using a to weight between them.
	 * @tparam T Type of the input and output.
	 * @param x Start of the range in which to interpolate.
	 * @param y End of the range in which to interpolate.
	 * @param a The value to use to interpolate between x and y.
	 * @return The interpolated value.
	 */
	template <typename T>
	constexpr T mix(T x, T y, T a) noexcept
	{
		// ReSharper disable once CppRedundantParentheses
		return (x * (1 - a)) + (y * a);
	}

	/**
	 * @brief Performs a linear interpolation between x and y using a to weight between them.
	 * @tparam TStart Type of the start of the range.
	 * @tparam TEnd Type of the end of the range.
	 * @tparam TAplha Type of the value to use to interpolate between x and y.
	 * @param x Start of the range in which to interpolate.
	 * @param y End of the range in which to interpolate.
	 * @param a The value to use to interpolate between x and y.
	 * @return The interpolated value.
	 */
	template <typename TStart, typename TEnd, typename TAplha>
	constexpr std::common_type_t<TStart, TEnd, TAplha> mix(TStart x, TEnd y, TAplha a) noexcept
	{
		return (x * (1 - a)) + (y * a);
	}
} // namespace ccm::ext
