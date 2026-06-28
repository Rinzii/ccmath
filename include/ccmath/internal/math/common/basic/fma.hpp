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

#include "ccmath/internal/support/fp/fma.hpp"

#include <limits>
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
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true> constexpr T fma(T x, T y, T z) noexcept // NOLINT(bugprone-exception-escape)
	{
		return support::fp::dispatch_fma(x, y, z);
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true> constexpr Integer fma(Integer x, Integer y, Integer z) noexcept
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
	template <typename T, typename U, typename V> constexpr auto fma(T x, U y, V z) noexcept
	{
		constexpr auto TCommon = std::numeric_limits<T>::epsilon() > 0 ? std::numeric_limits<T>::epsilon() : 1;
		constexpr auto UCommon = std::numeric_limits<U>::epsilon() > 0 ? std::numeric_limits<U>::epsilon() : 1;
		constexpr auto VCommon = std::numeric_limits<V>::epsilon() > 0 ? std::numeric_limits<V>::epsilon() : 1;

		using epsilon_type = std::common_type_t<decltype(TCommon), decltype(UCommon), decltype(VCommon)>;

		using shared_type = std::conditional_t<
			TCommon <= std::numeric_limits<epsilon_type>::epsilon() && TCommon <= UCommon,
			T,
			std::conditional_t<UCommon <= std::numeric_limits<epsilon_type>::epsilon() && UCommon <= TCommon,
							   U,
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
	constexpr auto fma(T x, U y, V z) noexcept
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
	constexpr double fma(double x, double y, double z) noexcept
	{
		return ccm::fma<double>(x, y, z);
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
