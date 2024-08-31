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
	/**
	 * @brief Returns the fractional part of a floating-point number.
	 * @tparam T Type of the input and output.
	 * @param x Value to get the fractional part of.
	 * @return The fractional part of the input.
	 */
    template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
    constexpr T fract(T x) noexcept
	{
        return x - ccm::floor(x);
	}
} // namespace ccm::ext
