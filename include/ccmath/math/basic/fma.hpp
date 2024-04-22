/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include <type_traits>

namespace ccm
{
	/**
	 * @brief Fused multiply-add operation.
	 * @tparam T Numeric type.
	 * @param x Floating-point or integer value.
	 * @param y Floating-point or integer value.
	 * @param z Floating-point or integer value.
	 * @return If successful, returns the value of x * y + z as if calculated to infinite precision and rounded once to fit the result type (or, alternatively,
	 * calculated as a single ternary floating-point operation).
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T fma(T x, T y, T z) noexcept
	{
		// Check for GCC 6.1 or later
#if defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__)
		if constexpr (std::is_same_v<T, float>) { return __builtin_fmaf(x, y, z); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_fma(x, y, z); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_fmal(x, y, z); }
		else { __builtin_fma(x, y, z); }
#else
		// Handle infinity
		if (CCM_UNLIKELY((x == static_cast<T>(0) && ccm::isinf(y)) || (y == T{0} && ccm::isinf(x)))) { return std::numeric_limits<T>::quiet_NaN(); }
		if (CCM_UNLIKELY(x * y == std::numeric_limits<T>::infinity() && z == -std::numeric_limits<T>::infinity()))
		{
			return std::numeric_limits<T>::infinity();
		}

		// Handle NaN
		if (CCM_UNLIKELY(ccm::isnan(x) || ccm::isnan(y))) { return std::numeric_limits<T>::quiet_NaN(); }
		if (CCM_UNLIKELY(ccm::isnan(z) && (x * y != 0 * std::numeric_limits<T>::infinity() || x * y != std::numeric_limits<T>::infinity() * 0)))
		{
			return std::numeric_limits<T>::quiet_NaN();
		}

		// If we don't have access to a builtin fma. Then just hope the compiler optimizes this.
		return (x * y) + z;
#endif
	}

	template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	constexpr T fma(T x, T y, T z) noexcept
	{
		return (x * y) + z;
	}

	/**
	 * @brief Fused multiply-add operation.
	 * @tparam T Floating-point or integer type converted to a common type.
	 * @tparam U Floating-point or integer type converted to a common type.
	 * @tparam V Floating-point or integer type converted to a common type.
	 * @param x Floating-point or integer value converted to a common type.
	 * @param y Floating-point or integer value converted to a common type.
	 * @param z Floating-point or integer value converted to a common type.
	 * @return If successful, returns the value of x * y + z as if calculated to infinite precision and rounded once to fit the result type (or, alternatively,
	 * calculated as a single ternary floating-point operation).
	 */
	template <typename T, typename U, typename V>
	constexpr auto fma(T x, U y, V z) noexcept
	{
		// If our type is an integer epsilon will be set to 0 by default.
		// Instead, set epsilon to 1 so that our type is always at least the widest floating point type.
		constexpr auto TCommon = std::numeric_limits<T>::epsilon() > 0 ? std::numeric_limits<T>::epsilon() : 1;
		constexpr auto UCommon = std::numeric_limits<U>::epsilon() > 0 ? std::numeric_limits<U>::epsilon() : 1;
		constexpr auto VCommon = std::numeric_limits<V>::epsilon() > 0 ? std::numeric_limits<V>::epsilon() : 1;

		using epsilon_type = std::common_type_t<decltype(TCommon), decltype(UCommon), decltype(VCommon)>;

		using shared_type = std::conditional_t<
			TCommon <= std::numeric_limits<epsilon_type>::epsilon() && TCommon <= UCommon, T,
			std::conditional_t<UCommon <= std::numeric_limits<epsilon_type>::epsilon() && UCommon <= TCommon, U,
							   std::conditional_t<VCommon <= std::numeric_limits<epsilon_type>::epsilon() && VCommon <= UCommon, V, epsilon_type>>>;

		return ccm::fma<shared_type>(static_cast<shared_type>(x), static_cast<shared_type>(y), static_cast<shared_type>(z));
	}

	/**
	 * @brief Fused multiply-add operation.
	 * @tparam T Integer type converted to a common type.
	 * @tparam U Integer type converted to a common type.
	 * @tparam V Integer type converted to a common type.
	 * @param x Integer value converted to a common type.
	 * @param y Integer value converted to a common type.
	 * @param z Integer value converted to a common type.
	 * @return If successful, returns the value of x * y + z as if calculated to infinite precision and rounded once to fit the result type (or, alternatively,
	 * calculated as a single ternary floating-point operation).
	 */
	template <typename T, typename U, typename V, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<U> && std::is_integral_v<V>, bool> = true>
	constexpr auto fma(T x, U y, V z) noexcept // Special case for if all types are integers.
	{
		using shared_type = std::common_type_t<T, U, V>;
		return ccm::fma<shared_type>(static_cast<shared_type>(x), static_cast<shared_type>(y), static_cast<shared_type>(z));
	}

	/**
	 * @brief Fused multiply-add operation.
	 * @param x Floating-point value.
	 * @param y Floating-point value.
	 * @param z Floating-point value.
	 * @return If successful, returns the value of x * y + z as if calculated to infinite precision and rounded once to fit the result type (or, alternatively,
	 * calculated as a single ternary floating-point operation).
	 */
	constexpr float fmaf(float x, float y, float z) noexcept
	{
		return ccm::fma<float>(x, y, z);
	}

	/**
	 * @brief Fused multiply-add operation.
	 * @param x Floating-point value.
	 * @param y Floating-point value.
	 * @param z Floating-point value.
	 * @return If successful, returns the value of x * y + z as if calculated to infinite precision and rounded once to fit the result type (or, alternatively,
	 * calculated as a single ternary floating-point operation).
	 */
	constexpr long double fmal(long double x, long double y, long double z) noexcept
	{
		return ccm::fma<long double>(x, y, z);
	}
} // namespace ccm

/// @ingroup basic
