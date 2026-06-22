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
#include "ccmath/internal/math/runtime/func/basic/fabs_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"

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
		if (!ccm::support::is_constant_evaluated()) { return ccm::rt::fabs_rt(num); }
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
		// If num is less than zero, return -num, otherwise return num. The explicit cast keeps the
		// result in T, where -num integer-promotes to int and would otherwise narrow on return for
		// types narrower than int such as signed char and short.
		return static_cast<T>(num < 0 ? -num : num);
	}

	/**
	 * @brief Computes the absolute value of an unsigned value.
	 * @tparam T Unsigned integer type.
	 * @param num Unsigned value.
	 * @return num unchanged. An unsigned value is already non-negative, so its absolute value is itself.
	 */
	template <typename T>
	constexpr auto abs(T num) -> std::enable_if_t<std::is_unsigned_v<T>, T>
	{
		// An unsigned value is already non-negative, so the absolute value is the value itself.
		// Returning num directly is lossless and type-preserving, where routing through int would
		// narrow wide unsigned values and trip -Wsign-conversion.
		return num;
	}

	/**
	 * @brief Computes the absolute value of a number.
	 * @tparam T Floating-point type.
	 * @param num Floating-point value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename T>
	constexpr auto fabs(T num) -> std::enable_if_t<std::is_floating_point_v<T>, T>
	{ return ccm::abs<T>(num); }

	/**
	 * @brief Computes the absolute value of a number.
	 * @tparam Integer Integer type.
	 * @param num Integer value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename Integer>
	constexpr auto fabs(Integer num) -> std::enable_if_t<std::is_integral_v<Integer>, double>
	{ return ccm::abs<double>(static_cast<double>(num)); }

	/**
	 * @brief Computes the absolute value of a number.
	 * @param num Floating-point value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	constexpr float fabsf(float num)
	{ return ccm::abs<float>(num); }

	/**
	 * @brief Computes the absolute value of a number.
	 * @param num Floating-point value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	constexpr long double fabsl(long double num)
	{ return ccm::abs<long double>(num); }

	/**
	 * @brief Computes the absolute value of a number.
	 * @param num Integer value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	constexpr long labs(long num)
	{ return ccm::abs<long>(num); }

	/**
	 * @brief Computes the absolute value of a number.
	 * @param num Integer value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	constexpr long long llabs(long long num)
	{ return ccm::abs<long long>(num); }
} // namespace ccm

/// @ingroup basic
