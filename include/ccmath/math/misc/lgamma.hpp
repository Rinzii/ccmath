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

#include "ccmath/internal/math/generic/builtins/misc/lgamma.hpp"
#include "ccmath/internal/math/runtime/func/misc/lgamma_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/misc/impl/lgamma_impl.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Computes the natural logarithm of the absolute value of the gamma function.
	 * @tparam T floating-point or integer type
	 * @param num floating-point or integer value
	 * @return If no errors occur, the natural logarithm of the absolute value of the gamma function at num is returned.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/lgamma
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T lgamma(T num)
	{
		if (!ccm::support::is_constant_evaluated()) { return ccm::rt::lgamma_rt(num); }

		if constexpr (std::is_same_v<T, float>) { return internal::lgamma_float(num); }
		if constexpr (std::is_same_v<T, double>) { return internal::lgamma_double(num); }
		if constexpr (std::is_same_v<T, long double>) { return static_cast<long double>(internal::lgamma_double(static_cast<double>(num))); }
		return static_cast<T>(internal::lgamma_double(static_cast<double>(num)));
	}

	/**
	 * @brief Computes log-gamma after promoting an integer input to double.
	 * @tparam Integer Integral type.
	 * @param num Integer value.
	 * @return Natural logarithm of |gamma(num)| as double.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/lgamma
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double lgamma(Integer num)
	{ return ccm::lgamma<double>(static_cast<double>(num)); }

	/**
	 * @brief Computes log-gamma for float.
	 * @param num Floating-point value.
	 * @return Natural logarithm of |gamma(num)| as float.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/lgamma
	 */
	constexpr float lgammaf(float num)
	{ return ccm::lgamma<float>(num); }

	/**
	 * @brief Computes log-gamma for long double.
	 * @param num Floating-point value.
	 * @return Natural logarithm of |gamma(num)| as long double.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/lgamma
	 */
	constexpr long double lgammal(long double num)
	{ return ccm::lgamma<long double>(num); }
} // namespace ccm
