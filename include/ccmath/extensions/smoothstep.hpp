/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/extensions/clamp.hpp"
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
    inline constexpr T smoothstep(T edge0, T edge1, T x)
    {
		// Scale, bias and saturate x to 0..1 range
        x = ccm::ext::clamp((x - edge0) / (edge1 - edge0));
        // Evaluate polynomial
        return x * x * (static_cast<T>(3) - static_cast<T>(2) * x);
    }
}
