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
	 * @brief Return the sign of a value.
	 * @tparam T Type of the input and output.
	 * @param value The value whose sign is queried.
	 * @return -1 if value is negative, 0 if value is zero, otherwise 1.
	 */
	template <typename T, std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, bool>, bool> = true> constexpr T sign(T value) noexcept
	{
		return static_cast<T>((T(0) < value) - (value < T(0)));
	}
} // namespace ccm::ext
