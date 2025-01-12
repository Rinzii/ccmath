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

#include "ccmath/internal/math/common/basic/fdim.hpp"

namespace ccm
{
	/**
	 * @brief Computes the positive difference of two floating point values (max(0,x−y))
	 * @tparam T A floating-point type.
	 * @param x A floating-point or integer values
	 * @param y A floating-point or integer values
	 * @return If successful, returns the positive difference between x and y.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T fdim(T x, T y)
	{
		return func::fdim(x, y);
	}

	/**
	 * @brief Computes the positive difference of two floating point values (max(0,x−y))
	 * @tparam T A floating-point type.
	 * @tparam U A floating-point type.
	 * @param x A floating-point value.
	 * @param y A floating-point value.
	 * @return If successful, returns the positive difference between x and y.
	 */
	template <typename T, typename U, std::enable_if_t<std::is_floating_point_v<T> && std::is_floating_point_v<U>, bool> = true>
	constexpr auto fdim(T x, U y)
	{
		// Find the common type of the two arguments
		using shared_type = std::common_type_t<T, U>;

		// Convert the arguments to the common type
		return ccm::fdim<shared_type>(static_cast<shared_type>(x), static_cast<shared_type>(y));
	}

	/**
	 * @brief Computes the positive difference of two floating point values (max(0,x−y))
	 * @tparam Integer An integral type.
	 * @param x An integral value.
	 * @param y An integral value.
	 * @return If successful, returns the positive difference between x and y.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double fdim(Integer x, Integer y)
	{
		return ccm::fdim<double>(static_cast<double>(x), static_cast<double>(y));
	}

	/**
	 * @brief Computes the positive difference of two floating point values (max(0,x−y))
	 * @param x A floating-point value.
	 * @param y A floating-point value.
	 * @return If successful, returns the positive difference between x and y.
	 */
	constexpr float fdimf(float x, float y)
	{
		return ccm::fdim<float>(x, y);
	}

	/**
	 * @brief Computes the positive difference of two floating point values (max(0,x−y))
	 * @param x A floating-point value.
	 * @param y A floating-point value.
	 * @return If successful, returns the positive difference between x and y.
	 */
	constexpr long double fdiml(long double x, long double y)
	{
		return ccm::fdim<long double>(x, y);
	}
} // namespace ccm

/// @ingroup basic
