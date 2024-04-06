/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Check if the given number is NaN.
	 * @tparam T The type of the number to check.
	 * @param x The number to check.
	 * @return True if the number is NaN, false otherwise.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, int> = 0>
	[[nodiscard]] constexpr bool isnan(T x) noexcept
	{
#if defined(__GNUC__) || defined(__clang__)
		return __builtin_isnan(x); // GCC and Clang implement this as constexpr
#else							   // If we can't use the builtin, fallback to this comparison and hope for the best.
		return x != x; // NOLINT
#endif
	}

	/**
	 * @brief Check if the given number is NaN.
	 * @tparam Integer The type of the number to check.
	 * @return False, as integers can never be NaN.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, int> = 0>
	[[nodiscard]] constexpr bool isnan(Integer /* x */)
	{
		return false; // Integers can never be NaN.
	}

} // namespace ccm

/// @ingroup compare
