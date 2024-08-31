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
	 * @brief Determines if a number is a power of two.
	 * @tparam T Type of the input.
	 * @param n Value to check.
	 * @return True if the input is a power of two, false otherwise.
	 */
	template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	constexpr bool is_power_of_two(T n) noexcept
	{
		// https://graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2
		return n && !(n & (n - 1));
	}
} // namespace ccm::ext
