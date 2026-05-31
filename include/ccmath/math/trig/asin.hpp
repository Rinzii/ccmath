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

#include "ccmath/internal/math/generic/builtins/trig/asin.hpp"
#include "ccmath/internal/math/runtime/func/trig/asin_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/trig/impl/inv_trig_impl.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Computes the principal value of the inverse sine in radians.
	 * @tparam T Floating-point type.
	 * @param num Floating-point value, typically in [-1, 1].
	 * @return Inverse sine of num in radians.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/asin
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T asin(T num)
	{
		if constexpr (ccm::builtin::has_constexpr_asin<T>) { return ccm::builtin::asin(num); }
		else if (ccm::support::is_constant_evaluated())
		{
			if constexpr (std::is_same_v<T, float>) { return internal::impl::asin_float(num); }
			else if constexpr (std::is_same_v<T, double>) { return internal::impl::asin_double(num); }
			else { return static_cast<long double>(internal::impl::asin_double(static_cast<double>(num))); }
		}
		else { return ccm::rt::asin_rt(num); }
	}

	/**
	 * @brief Computes inverse sine of an integer input after promotion to double.
	 * @tparam Integer Integral type.
	 * @param num Input value.
	 * @return Inverse sine in radians as double.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/asin
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double asin(Integer num)
	{
		return ccm::asin<double>(static_cast<double>(num));
	}

	/**
	 * @brief Computes inverse sine for float.
	 * @param num Input value.
	 * @return Inverse sine in radians as float.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/asin
	 */
	constexpr float asinf(float num)
	{
		return ccm::asin<float>(num);
	}

	/**
	 * @brief Computes inverse sine for long double.
	 * @param num Input value.
	 * @return Inverse sine in radians as long double.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/asin
	 */
	constexpr long double asinl(long double num)
	{
		return ccm::asin<long double>(num);
	}
} // namespace ccm

/// @ingroup trig
