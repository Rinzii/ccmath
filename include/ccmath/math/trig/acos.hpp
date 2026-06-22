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

#include "ccmath/internal/math/generic/builtins/trig/acos.hpp"
#include "ccmath/internal/math/runtime/func/trig/acos_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/trig/impl/inv_trig_impl.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Computes the principal value of the inverse cosine in radians.
	 * @tparam T Floating-point type.
	 * @param num Floating-point value, typically in [-1, 1].
	 * @return Inverse cosine of num in radians.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T acos(T num)
	{
		if constexpr (ccm::builtin::has_constexpr_acos<T>) { return ccm::builtin::acos_ct(num); }
		else if (ccm::support::is_constant_evaluated())
		{
			if constexpr (std::is_same_v<T, float>) { return internal::impl::acos_float(num); }
			else if constexpr (std::is_same_v<T, double>) { return internal::impl::acos_double(num); }
			else
			{
				return static_cast<long double>(internal::impl::acos_double(static_cast<double>(num)));
			}
		}
		else
		{
			return ccm::rt::acos_rt(num);
		}
	}

	/**
	 * @brief Computes inverse cosine of an integer input after promotion to double.
	 * @tparam Integer Integral type.
	 * @param num Input value.
	 * @return Inverse cosine in radians as double.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double acos(Integer num)
	{ return ccm::acos<double>(static_cast<double>(num)); }

	/**
	 * @brief Computes inverse cosine for float.
	 * @param num Input value.
	 * @return Inverse cosine in radians as float.
	 */
	constexpr float acosf(float num)
	{ return ccm::acos<float>(num); }

	/**
	 * @brief Computes inverse cosine for long double.
	 * @param num Input value.
	 * @return Inverse cosine in radians as long double.
	 */
	constexpr long double acosl(long double num)
	{ return ccm::acos<long double>(num); }
} // namespace ccm

/// @ingroup trig
