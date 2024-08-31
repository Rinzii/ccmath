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

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Computes the smaller of the two values.
	 * @tparam T Type of the values to compare.
	 * @param x Left-hand side of the comparison.
	 * @param y Right-hand side of the comparison.
	 * @return If successful, returns the smaller of two floating point values. The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename T>
	constexpr T min(const T x, const T y) noexcept
	{
		// If we are comparing floating point numbers, we need to check for NaN
		if constexpr (std::is_floating_point_v<T>)
		{
			using FPBits_t = typename ccm::support::fp::FPBits<T>;
			const FPBits_t x_bits(x);
			const FPBits_t y_bits(y);

			const bool x_is_nan = x_bits.is_nan();
			const bool y_is_nan = y_bits.is_nan();

			if (CCM_UNLIKELY(x_is_nan)) { return y; }
			if (CCM_UNLIKELY(y_is_nan)) { return x; }
		}

		return (x < y) ? x : y;
	}

	/**
	 * @brief Computes the smaller of the two values.
	 * @tparam T Left-hand type of the left-hand value to compare.
	 * @tparam U Right-hand type of the right-hand value to compare.
	 * @param x Left-hand side of the comparison.
	 * @param y Right-hand side of the comparison.
	 * @return If successful, returns the smaller of two floating point values. The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename T, typename U>
	constexpr T min(const T x, const U y) noexcept
	{
		// Find the common type of the two arguments
		using shared_type = std::common_type_t<T, U>;

		// Then cast the arguments to the common type and call the single argument version
		return static_cast<shared_type>(min<shared_type>(static_cast<shared_type>(x), static_cast<shared_type>(y)));
	}

	/**
	 * @brief Computes the smaller of the two values.
	 * @tparam Real Type of of the values to compare.
	 * @param x Left-hand side of the comparison.
	 * @param y Right-hand side of the comparison.
	 * @return If successful, returns the smaller of two floating point values. The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename Real, std::enable_if_t<!std::is_integral_v<Real>, bool> = true>
	constexpr Real fmin(const Real x, const Real y) noexcept
	{
		return min<Real>(x, y);
	}

	/**
	 * @brief Computes the smaller of the two values.
	 * @tparam T Left-hand type of the left-hand value to compare.
	 * @tparam U Right-hand type of the right-hand value to compare.
	 * @param x Left-hand side of the comparison.
	 * @param y Right-hand side of the comparison.
	 * @return If successful, returns the smaller of two floating point values. The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename T, typename U>
	constexpr auto fmin(const T x, const U y) noexcept
	{
		// Find the common type of the two arguments
		using shared_type = std::common_type_t<T, U>;

		// Then cast the arguments to the common type and call the single argument version
		return static_cast<shared_type>(min<shared_type>(static_cast<shared_type>(x), static_cast<shared_type>(y)));
	}

	/**
	 * @brief Computes the smaller of the two values.
	 * @tparam Integer An integral type.
	 * @param x Right-hand side of the comparison.
	 * @param y Left-hand side of the comparison.
	 * @return If successful, returns the smaller of two floating point values. The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr Integer fmin(const Integer x, const Integer y) noexcept
	{
		return min<Integer>(x, y);
	}
} // namespace ccm

/// @ingroup basic
