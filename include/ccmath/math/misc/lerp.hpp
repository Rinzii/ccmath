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

#include "ccmath/math/basic/fma.hpp"
#include "ccmath/math/compare/isnan.hpp"

namespace ccm
{
	/**
	 * @brief Performs linear interpolation between a and b using t.
	 * @tparam T Arithmetic type.
	 * @param a Start value.
	 * @param b End value.
	 * @param t Interpolation factor.
	 * @return Interpolated value equivalent to a + t * (b - a) with stability handling for edge cases.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/lerp
	 */
	template <typename T>
	constexpr T lerp(T a, T b, T t) noexcept
	{
		if (ccm::isnan(a) || ccm::isnan(b) || ccm::isnan(t)) { return a + b + t; }

		if ((a <= 0 && b >= 0) || (a >= 0 && b <= 0)) { return t * b + (1 - t) * a; }

		if (t == 1) { return b; }

		const T x = a + t * (b - a);
		if ((t > 1) == (b > a)) { return b < x ? x : b; }

		return x < b ? x : b;
	}

	/**
	 * @brief Performs linear interpolation after promoting arguments to a common arithmetic type.
	 * @tparam T Arithmetic type of a.
	 * @tparam U Arithmetic type of b.
	 * @tparam V Arithmetic type of t.
	 * @param a Start value.
	 * @param b End value.
	 * @param t Interpolation factor.
	 * @return Interpolated value in the common type of T, U, and V.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/lerp
	 */
	template <typename T, typename U, typename V>
	constexpr std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U> && std::is_arithmetic_v<V>, std::common_type_t<T, U, V>>
	lerp(T a, U b, V t) noexcept
	{
		using result_type = std::common_type_t<T, U, V>;
		static_assert(!(std::is_same_v<T, result_type> && std::is_same_v<U, result_type> && std::is_same_v<V, result_type>));
		return lerp(static_cast<result_type>(a), static_cast<result_type>(b), static_cast<result_type>(t));
	}

	// TODO: Remove this once we confirm the new lerp is 100% stable
	/**
	 * @brief Deprecated fallback implementation of linear interpolation.
	 * @tparam T Arithmetic type.
	 * @param a Start value.
	 * @param b End value.
	 * @param t Interpolation factor.
	 * @return Interpolated value from the previous implementation.
	 * @deprecated Use ccm::lerp instead.
	 */
	template <typename T>
	[[maybe_unused]] [[deprecated(
		"Do not use ccm::lerp_old it is only being kept as a fallback until ccm::lerp has been validated as conforming to std::lerp")]] constexpr T
	old_lerp(T a, T b, T t) noexcept
	{
		if ((a <= 0 && b >= 0) || (a >= 0 && b <= 0)) { return t * b + (1 - t) * a; }

		if (t == 1) { return b; }

		const T x = a + t * (b - a);
		if ((t > 1) == (b > a)) { return b < x ? x : b; }

		return x < b ? x : b;
	}
} // namespace ccm
