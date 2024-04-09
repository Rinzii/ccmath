/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/nearest/floor.hpp"
#include <type_traits>

namespace ccm::ext
{
    template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
    constexpr T fract(T x) noexcept
	{
        return x - ccm::floor(x);
	}
} // namespace ccm
