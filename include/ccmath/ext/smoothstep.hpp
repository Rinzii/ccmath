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
    /**
     * @brief Smooth hermite interpolation.
     * @tparam T Type of the input and output.
     * @param edge0 Lower edge.
     * @param edge1 Upper edge.
     * @param x Value to interpolate.
     * @return The interpolated value.
     */
    template<typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
    constexpr T smoothstep(T edge0, T edge1, T x)
    {
		// Scale, bias and saturate x to 0..1 range
        x = ccm::ext::clamp((x - edge0) / (edge1 - edge0));
        // Evaluate polynomial
        return x * x * (static_cast<T>(3) - static_cast<T>(2) * x);
    }
} // namespace ccm::ext
