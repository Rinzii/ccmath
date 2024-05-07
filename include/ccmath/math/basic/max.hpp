/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present cmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include <limits>
#include <type_traits>

namespace ccm
{
	/**
	 * @brief Computes the larger of the two values.
	 * @tparam T Type of the values to compare.
	 * @param x Left-hand side of the comparison.
	 * @param y Right-hand side of the comparison.
	 * @return If successful, returns the larger of two floating point values. The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename T>
	constexpr T max(T x, T y) noexcept
	{
		if constexpr (std::is_floating_point_v<T>)
		{
			if (CCM_UNLIKELY(ccm::isnan(x) && ccm::isnan(y))) { return std::numeric_limits<T>::quiet_NaN(); }

			if (CCM_UNLIKELY(ccm::isnan(x))) { return y; }

			if (CCM_UNLIKELY(ccm::isnan(y))) { return x; }
		}

		return (x > y) ? x : y;
	}

	/**
	 * @brief Computes the larger of the two values.
	 * @tparam T Type of left-hand side of the comparison.
	 * @tparam U Type of right-hand side of the comparison.
	 * @param x Left-hand side of the comparison.
	 * @param y Right-hand side of the comparison.
	 * @return
	 */
	template <typename T, typename U>
	constexpr auto max(T x, U y) noexcept
	{
		// Find the common type of the two arguments
		using shared_type = std::common_type_t<T, U>;

		// Then cast the arguments to the common type and call the single argument version
		return static_cast<shared_type>(max(static_cast<shared_type>(x), static_cast<shared_type>(y)));
	}

	/**
	 * @brief Computes the larger of the two floating point values.
	 * @tparam T Type of the values to compare.
	 * @param x Left-hand side of the comparison.
	 * @param y Right-hand side of the comparison.
	 * @return If successful, returns the larger of two floating point values. The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename T>
	constexpr T fmax(T x, T y) noexcept
	{
		return max<T>(x, y);
	}

	/**
	 * @brief Computes the larger of the two values.
	 * @tparam T Type of left-hand side of the comparison.
	 * @tparam U Type of right-hand side of the comparison.
	 * @param x Left-hand side of the comparison.
	 * @param y Right-hand side of the comparison.
	 * @return If successful, returns the larger of two floating point values. The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename T, typename U>
	constexpr auto fmax(T x, U y) noexcept
	{
		// Find the common type of the two arguments
		using shared_type = std::common_type_t<T, U>;

		// Then cast the arguments to the common type and call the single argument version
		return static_cast<shared_type>(max<shared_type>(static_cast<shared_type>(x), static_cast<shared_type>(y)));
	}

	/**
	 * @brief Computes the larger of the two floating point values.
	 * @param x Left-hand side of the comparison.
	 * @param y Right-hand side of the comparison.
	 * @return If successful, returns the larger of two floating point values. The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double fmax(Integer x, Integer y) noexcept
	{
		return max<double>(static_cast<double>(x), static_cast<double>(y));
	}

	/**
	 * @brief Computes the larger of the two floating point values.
	 * @param x Left-hand side of the comparison.
	 * @param y Right-hand side of the comparison.
	 * @return If successful, returns the larger of two floating point values. The value returned is exact and does not depend on any rounding modes.
	 */
	constexpr float fmaxf(float x, float y) noexcept
	{
		return fmax<float>(x, y);
	}

	/**
	 * @brief Computes the larger of the two floating point values.
	 * @param x Left-hand side of the comparison.
	 * @param y Right-hand side of the comparison.
	 * @return If successful, returns the larger of two floating point values. The value returned is exact and does not depend on any rounding modes.
	 */
	constexpr long double fmaxl(long double x, long double y) noexcept
	{
		return fmax<long double>(x, y);
	}
} // namespace ccm

/// @ingroup basic
