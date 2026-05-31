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

#include "ccmath/ext/fract.hpp"
#include "ccmath/math/basic/fabs.hpp"

#include <type_traits>

namespace ccm::ext
{
	/**
	 * @brief Ping-pong a value between 0 and a specified range.
	 * @tparam T Type of the input.
	 * @param a Value to ping-pong.
	 * @param b Range to ping-pong within.
	 * @return The ping-ponged value.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T ping_pong(T a, T b) noexcept
	{
		if (b == 0) { return T(0); }

		return ccm::abs((ccm::ext::fract((a - b) / (b * 2.0)) * b * 2.0) - b);
	}
} // namespace ccm::ext
