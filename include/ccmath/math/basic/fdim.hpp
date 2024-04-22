/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/compare/isnan.hpp"
#include <limits>
#include <type_traits>

namespace ccm
{
	/**
	 * @brief Computes the positive difference of two floating point values (max(0,x−y))
	 * @tparam T A floating-point type.
	 * @param x A floating-point or integer values
	 * @param y A floating-point or integer values
	 * @return If successful, returns the positive difference between x and y.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
	constexpr T fdim(T x, T y)
	{
		if (CCM_UNLIKELY(ccm::isnan(x))) { return x; }
		if (CCM_UNLIKELY(ccm::isnan(y))) { return y; }
		if (x <= y) { return static_cast<T>(+0.0); }
		if (y < static_cast<T>(0.0) && x > std::numeric_limits<T>::max() + y) { return std::numeric_limits<T>::infinity(); }
		return x - y;
	}

	/**
	 * @brief Computes the positive difference of two floating point values (max(0,x−y))
	 * @tparam T A floating-point type.
	 * @tparam U A floating-point type.
	 * @param x A floating-point value.
	 * @param y A floating-point value.
	 * @return If successful, returns the positive difference between x and y.
	 */
	template <typename T, typename U, std::enable_if_t<std::is_floating_point_v<T> && std::is_floating_point_v<U>, int> = 0>
	constexpr auto fdim(T x, U y)
	{
		// Find the common type of the two arguments
		using shared_type = std::common_type_t<T, U>;

		// Convert the arguments to the common type
		return fdim<shared_type>(static_cast<shared_type>(x), static_cast<shared_type>(y));
	}

	/**
	 * @brief Computes the positive difference of two floating point values (max(0,x−y))
	 * @tparam Integer An integral type.
	 * @param x An integral value.
	 * @param y An integral value.
	 * @return If successful, returns the positive difference between x and y.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, int> = 0>
	constexpr double fdim(Integer x, Integer y)
	{
		return fdim<double>(static_cast<double>(x), static_cast<double>(y));
	}

	/**
	 * @brief Computes the positive difference of two floating point values (max(0,x−y))
	 * @param x A floating-point value.
	 * @param y A floating-point value.
	 * @return If successful, returns the positive difference between x and y.
	 */
	constexpr float fdimf(float x, float y)
	{
		return fdim<float>(x, y);
	}

	/**
	 * @brief Computes the positive difference of two floating point values (max(0,x−y))
	 * @param x A floating-point value.
	 * @param y A floating-point value.
	 * @return If successful, returns the positive difference between x and y.
	 */
	constexpr long double fdiml(long double x, long double y)
	{
		return fdim<long double>(x, y);
	}
} // namespace ccm

/// @ingroup basic
