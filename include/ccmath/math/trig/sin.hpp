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

#include "ccmath/internal/math/generic/builtins/trig/sin.hpp"
#include "ccmath/internal/math/generic/func/trig/sin_gen.hpp"
#include "ccmath/internal/math/runtime/func/trig/sin_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Computes the sine of an angle in radians.
	 * @tparam T Floating-point type.
	 * @param num Angle in radians.
	 * @return Sine of num.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/sin
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T sin(T num)
	{
		if constexpr (ccm::builtin::has_constexpr_sin<T>) { return ccm::builtin::sin_ct(num); }
		else
		{
			if (ccm::support::is_constant_evaluated()) { return ccm::gen::sin_gen(num); }
			return ccm::rt::sin_rt(num);
		}
	}

	/**
	 * @brief Computes the sine of an integral angle value after promotion to double.
	 * @tparam Integer Integral type.
	 * @param num Angle in radians.
	 * @return Sine of num as double.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/sin
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double sin(Integer num)
	{ return ccm::sin<double>(static_cast<double>(num)); }

	/**
	 * @brief Computes the sine of a float angle in radians.
	 * @param num Angle in radians.
	 * @return Sine as float.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/sin
	 */
	constexpr float sinf(float num)
	{ return ccm::sin<float>(num); }

	/**
	 * @brief Computes the sine of a long double angle in radians.
	 * @param num Angle in radians.
	 * @return Sine as long double.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/sin
	 */
	constexpr long double sinl(long double num)
	{ return ccm::sin<long double>(num); }
} // namespace ccm

/// @ingroup trig
