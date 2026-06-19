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

#include "ccmath/internal/support/bits.hpp"

#include <type_traits>

namespace ccm::ext
{
	/**
	 * @brief Determines if an unsigned integer value is a power of two.
	 *
	 * @tparam T Type of the input.
	 * @param n Value to check.
	 * @return True if the input is a power of two, false otherwise.
	 */
	template <typename T, std::enable_if_t<std::is_unsigned_v<T> && !std::is_same_v<T, bool>, bool> = true>
	constexpr bool is_power_of_two(T n) noexcept
	{ return support::has_single_bit(n); }

	/**
	 * @brief Determines if a signed integer value is a power of two.
	 *
	 * Negative values and zero are never a power of two. The positive check is
	 * done on the unsigned value, so the minimum representable value does not
	 * trigger signed overflow.
	 *
	 * @tparam T Type of the input.
	 * @param n Value to check.
	 * @return True if the input is a power of two, false otherwise.
	 */
	template <typename T, std::enable_if_t<std::is_signed_v<T> && !std::is_same_v<T, bool>, bool> = true>
	constexpr bool is_power_of_two(T n) noexcept
	{
		using unsigned_type = std::make_unsigned_t<T>;

		return n > T(0) && support::has_single_bit(static_cast<unsigned_type>(n));
	}

	/**
	 * @brief Alias for is_power_of_two.
	 *
	 * @tparam T Type of the input.
	 * @param n Value to check.
	 * @return True if the input is a power of two, false otherwise.
	 */
	template <typename T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, bool> = true>
	constexpr bool ispow2(T n) noexcept
	{ return is_power_of_two(n); }
} // namespace ccm::ext
