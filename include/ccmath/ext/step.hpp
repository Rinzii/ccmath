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
	/**
	 * @brief Return 0 if value is less than edge, otherwise return 1.
	 * @tparam T Type of the input and output.
	 * @param edge The comparison edge.
	 * @param value The value to compare against edge.
	 * @return 0 if value is less than edge, otherwise 1.
	 */
	template <typename T, std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, bool>, bool> = true>
	constexpr T step(T edge, T value) noexcept
	{ return value < edge ? T(0) : T(1); }
} // namespace ccm::ext
