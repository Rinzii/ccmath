/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present cmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/isnan.hpp"

#include <limits>

namespace ccm
{
	/**
	 * @brief Computes the absolute value of a number.
	 * @tparam T Numeric type.
	 * @param num Floating-point or integer value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T> && !std::is_unsigned_v<T>, bool> = true>
	constexpr T abs(T num) noexcept
	{
		// If num is NaN, return a quiet NaN.
		if (CCM_UNLIKELY(ccm::isnan<T>(num))) { return std::numeric_limits<T>::quiet_NaN(); }

		// If num is equal to ±zero, return +zero.
		if (static_cast<T>(0) == num) { return static_cast<T>(0); }

		// If num is less than zero, return -num, otherwise return num.
		return num < 0 ? -num : num;
	}

	/**
	 * @brief Computes the absolute value of a number.
	 * @tparam T Numeric type.
	 * @param num Floating-point or integer value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename T, std::enable_if_t<std::is_integral_v<T> && !std::is_unsigned_v<T>, bool> = true>
	constexpr T abs(T num) noexcept
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
	template <typename T, std::enable_if_t<std::is_unsigned_v<T>, bool> = true>
	constexpr T abs(T num) noexcept
	{
		// If abs is called with an argument of type X for which is_unsigned_v<X> is true, and
		// if X cannot be converted to int by integral promotion, the program is ill-formed.
		// See: http://eel.is/c++draft/c.math.abs#3
		// See: ISO/IEC 9899:2018, 7.12.7.2, 7.22.6.1
		if constexpr (std::is_convertible_v<T, int>) { return abs<int>(static_cast<int>(num)); }
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
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T fabs(T num) noexcept
	{
		return ccm::abs<T>(num);
	}

	/**
	 * @brief Computes the absolute value of a number.
	 * @tparam Integer Integer type.
	 * @param num Integer value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double fabs(Integer num) noexcept
	{
		return ccm::abs<double>(static_cast<double>(num));
	}

	/**
	 * @brief Computes the absolute value of a number.
	 * @param num Floating-point value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	constexpr float fabsf(float num) noexcept
	{
		return ccm::abs<float>(num);
	}

	/**
	 * @brief Computes the absolute value of a number.
	 * @param num Floating-point value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	constexpr long double fabsl(long double num) noexcept
	{
		return ccm::abs<long double>(num);
	}

	/**
	 * @brief Computes the absolute value of a number.
	 * @param num Integer value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	constexpr long labs(long num) noexcept
	{
		return ccm::abs<long>(num);
	}

	/**
	 * @brief Computes the absolute value of a number.
	 * @param num Integer value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	constexpr long long llabs(long long num) noexcept
	{
		return ccm::abs<long long>(num);
	}
} // namespace ccm

/// @ingroup basic
