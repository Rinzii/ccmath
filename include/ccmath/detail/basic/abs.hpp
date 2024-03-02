/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present cmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/detail/compare/isnan.hpp"
#include "ccmath/internal/utility/unreachable.hpp"

#include <limits>

namespace ccm
{
	/**
	 * @brief Computes the absolute value of a number.
	 * @tparam T A numeric type.
	 * @param x A floating-point or integer value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename T>
	constexpr T abs(T x) noexcept
	{
		// If we are NOT dealing with an integral type check for NaN.
		if constexpr (!std::is_integral_v<T>)
		{
			// If x is NaN, return a quiet NaN.
			if (ccm::isnan<T>(x)) { return std::numeric_limits<T>::quiet_NaN(); }
		}

		// If x is equal to ±zero, return +zero.
		// Otherwise, if x is less than zero, return -x, otherwise return x.
		return x >= T{0} ? x : -x;
	}

	/**
	 * @brief Computes the absolute value of a number.
	 * @tparam T An unsigned numeric type.
	 * @param x A floating-point or integer value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename T, std::enable_if_t<std::is_unsigned_v<T>, bool> = true>
	constexpr T abs(T x) noexcept
	{
		// If abs is called with an argument of type X for which is_unsigned_v<X> is true and
		// if X cannot be converted to int by integral promotion, the program is ill-formed.
		// See: http://eel.is/c++draft/c.math.abs#3
		// See: ISO/IEC 9899:2018, 7.12.7.2, 7.22.6.1
		if constexpr (std::is_same_v<decltype(+x), int>) { return abs<int>(static_cast<int>(x)); }
		else
		{
			static_assert(sizeof(T) == 0, "Taking the absolute value of an unsigned type that cannot be converted to int by integral promotion is ill-formed.");
			ccm::unreachable(); // Give the compiler a hint that this branch is unreachable.
		}
	}

	/**
	 * @brief Computes the absolute value of a number.
	 * @tparam T A floating-point type.
	 * @param x A floating-point value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T fabs(T x) noexcept
	{
		return abs<T>(x);
	}

	/**
	 * @brief Computes the absolute value of a number.
	 * @param x A floating-point value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	inline constexpr float fabsf(float x) noexcept
	{
		return abs<float>(x);
	}

	/**
	 * @brief Computes the absolute value of a number.
	 * @param x A floating-point value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	inline constexpr long double fabsl(long double x) noexcept
	{
		return abs<long double>(x);
	}
} // namespace ccm

/// @ingroup basic
