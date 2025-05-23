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

#include "ccmath/internal/math/generic/builtins/compare/signbit.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/floating_point_traits.hpp"

namespace ccm
{
	/**
	 * @brief Detects the sign bit of zeroes, infinities, and NaNs.
	 * @tparam T A floating-point type.
	 * @param num A floating-point number.
	 * @return true if \p x is negative, false otherwise.
	 *
	 * @note This function has multiple implementations based on the compiler and the version of the compiler used.
	 * With nearly all implementations, this function is fully constexpr and will return
	 * the same values as std::signbit along with being static_assert-able.
	 *
	 * @warning ccm::signbit may fail
	 * to compile if you are not using the cmake build for ccmath or do not have access to __builtin_bit_cast.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] constexpr bool signbit(T num) noexcept
	{
		if constexpr (builtin::has_constexpr_signbit<T>)
		{
			return builtin::signbit(num);
		}
		else
		{
			// Check for the sign of +0.0 and -0.0 with bit_cast
			if (num == static_cast<T>(0) || ccm::isnan(num))
			{
				const auto bits = support::bit_cast<support::float_bits_t<T>>(num);
				return (bits & support::sign_mask_v<T>) != 0;
			}

			return num < static_cast<T>(0);
		}
	}

	/**
	 * @brief Detects the sign bit of a number.
	 * @tparam Integer An integral type.
	 * @param num An integral number.
	 * @return true if \p x is negative, false otherwise.
	 *
	 * @note This function is constexpr and will return the same values as std::signbit along with being static_assert-able.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer> && std::is_signed_v<Integer>, bool> = true>
	[[nodiscard]] constexpr bool signbit(Integer num) noexcept
	{
		// There is no concept of -0 for integers. So we can just check if the number is less than 0.
		return num < 0;
	}

	/**
	 * @brief Detects the sign bit of a number.
	 * @tparam Integer An integral type.
	 * @return false as an unsigned number can't be negative.
	 *
	 * @note This function is constexpr and will return the same values as std::signbit along with being static_assert-able.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer> && !std::is_signed_v<Integer>, bool> = true>
	[[nodiscard]] constexpr bool signbit(Integer /* num */) noexcept
	{
		// If the number is unsigned, then it can't be negative.
		return false;
	}

} // namespace ccm
