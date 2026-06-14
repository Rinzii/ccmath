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
	 * @brief Compute the absolute difference between two integral values.
	 * @tparam T Integral type of both inputs.
	 * @param lhs The first value.
	 * @param rhs The second value.
	 * @return The absolute difference between lhs and rhs as the matching unsigned type.
	 */
	template <typename T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<std::remove_cv_t<T>, bool>, bool> = true>
	[[nodiscard]] constexpr std::make_unsigned_t<T> abs_diff(T lhs, T rhs) noexcept
	{
		using U = std::make_unsigned_t<T>;
		return lhs > rhs ? static_cast<U>(static_cast<U>(lhs) - static_cast<U>(rhs)) : static_cast<U>(static_cast<U>(rhs) - static_cast<U>(lhs));
	}
} // namespace ccm::ext