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
	 * @brief Calculates a fast, approximate, reciprocal.
	 * @tparam T Type of the input and output.
	 * @param x Value to get the reciprocal of.
	 * @return The reciprocal of the input.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T rcp(T x) noexcept
	{
		return static_cast<T>(1) / x;
	}
} // namespace ccm::ext
