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

#include "ccmath/math/numbers.hpp"

#include <type_traits>

namespace ccm::ext
{
	/**
	 * @brief Converts an angle in radians to degrees.
	 * @tparam T Floating-point type.
	 * @param radians Angle in radians.
	 * @return Angle in degrees.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
    constexpr T degrees(T radians) noexcept
    {
        return (static_cast<T>(180) * radians) / ccm::numbers::pi_v<T>;
    }
} // namespace ccm::ext
