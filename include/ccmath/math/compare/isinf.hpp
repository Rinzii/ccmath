/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <limits>
#include <type_traits>

namespace ccm
{
	/**
	 * @brief Checks if the given number is infinite.
	 * @tparam T The type of the number to check.
	 * @param x The number to check.
	 * @return True if the number is infinite, false otherwise.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	inline constexpr bool isinf(T x) noexcept
	{
		if constexpr (std::numeric_limits<T>::is_signed) { return x == -std::numeric_limits<T>::infinity() || x == std::numeric_limits<T>::infinity(); }
		else { return x == std::numeric_limits<T>::infinity(); }
	}

	/**
	 * @brief Checks if the given number is infinite.
	 * @tparam Integer The type of the integer to check.
	 * @param x The integer to check.
	 * @return True if the number is infinite, false otherwise.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	inline constexpr bool isinf(Integer /* x */) noexcept
	{
		return false; // Integers cannot be infinite
	}
} // namespace ccm
