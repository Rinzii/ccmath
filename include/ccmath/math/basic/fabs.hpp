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

#include "ccmath/internal/math/common/basic/fabs.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Computes the absolute value of a number.
	 * @tparam T Numeric type.
	 * @param num Floating-point or integer value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename T>
	constexpr auto abs(T num) -> std::enable_if_t<std::is_floating_point_v<T> && std::is_signed_v<T>, T>
	{
		return func::fabs(num);
	}

	/**
	 * @brief Computes the absolute value of a number.
	 * @tparam T Numeric type.
	 * @param num Floating-point or integer value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename T>
	constexpr auto abs(T num) -> std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>, T>
	{
		// If num is less than zero, return -num, otherwise return num.
		return num < 0 ? -num : num;
	}

	/**
	 * @brief Computes the absolute value of a number.
	 * @tparam T Unsigned numeric type.
	 * @param num Floating-point or integer value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename T>
	constexpr auto abs(T num) -> std::enable_if_t<std::is_unsigned_v<T>, T>
	{
		// If abs is called with an argument of type X for which is_unsigned_v<X> is true, and
		// if X cannot be converted to int by integral promotion, the program is ill-formed.
		// See: http://eel.is/c++draft/c.math.abs#3
		// See: ISO/IEC 9899:2018, 7.12.7.2, 7.22.6.1
		if constexpr (std::is_convertible_v<T, int>) { return ccm::abs<int>(static_cast<int>(num)); }
		else
		{
			static_assert(sizeof(T) == 0, "Taking the absolute value of an unsigned type that cannot be converted to int by integral promotion is ill-formed.");
			return static_cast<T>(0);
		}
	}

	/**
	 * @brief Computes the absolute value of a number.
	 * @tparam T Floating-point type.
	 * @param num Floating-point value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename T>
	constexpr auto fabs(T num) -> std::enable_if_t<std::is_floating_point_v<T>, T>
	{
		return ccm::abs<T>(num);
	}

	/**
	 * @brief Computes the absolute value of a number.
	 * @tparam Integer Integer type.
	 * @param num Integer value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename Integer>
	constexpr auto fabs(Integer num) -> std::enable_if_t<std::is_integral_v<Integer>, double>
	{
		return ccm::abs<double>(static_cast<double>(num));
	}

	/**
	 * @brief Computes the absolute value of a number.
	 * @param num Floating-point value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	constexpr float fabsf(float num)
	{
		return ccm::abs<float>(num);
	}

	/**
	 * @brief Computes the absolute value of a number.
	 * @param num Floating-point value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	constexpr long double fabsl(long double num)
	{
		return ccm::abs<long double>(num);
	}

	/**
	 * @brief Computes the absolute value of a number.
	 * @param num Integer value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	constexpr long labs(long num)
	{
		return ccm::abs<long>(num);
	}

	/**
	 * @brief Computes the absolute value of a number.
	 * @param num Integer value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	constexpr long long llabs(long long num)
	{
		return ccm::abs<long long>(num);
	}
} // namespace ccm

/// @ingroup basic
