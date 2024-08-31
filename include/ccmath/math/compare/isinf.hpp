/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/fp/fp_bits.hpp"

#include <limits>
#include <type_traits>

namespace ccm
{
	/**
	 * @brief Checks if the given number is infinite.
	 * @tparam T The type of the number to check.
	 * @param num The number to check.
	 * @return True if the number is infinite, false otherwise.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr bool isinf(T num) noexcept
	{
#if defined(__GNUC__) || defined(__clang__)
		return __builtin_isinf(num); // GCC and Clang implement this as constexpr
#else							   // If we can't use the builtin, fallback to this comparison and hope for the best.
		using FPBits_t = typename ccm::support::fp::FPBits<T>;
		const FPBits_t num_bits(num);
		return num_bits.is_inf();
#endif
	}

	/**
	 * @brief Checks if the given number is infinite.
	 * @tparam Integer The type of the integer to check.
	 * @return True if the number is infinite, false otherwise.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr bool isinf(Integer /* num */) noexcept
	{
		return false; // Integers cannot be infinite
	}
} // namespace ccm
