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
	 * @brief Saturate a floating-point value to the range [0, 1].
	 * @tparam T Type of the input and output.
	 * @param value The value to saturate.
	 * @return The value clamped to the range [0, 1].
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] constexpr T saturate(T value) noexcept
	{
		return ext::clamp(value, T(0), T(1));
	}
} // namespace ccm::ext