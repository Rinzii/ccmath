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
	 * @brief Converts an angle in degrees to radians.
	 * @tparam T Floating-point type.
	 * @param degrees Angle in degrees.
	 * @return The angle in radians.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T radians(T degrees) noexcept
	{
		return (ccm::numbers::pi_v<T> * degrees) / static_cast<T>(180);
	}
} // namespace ccm::ext
