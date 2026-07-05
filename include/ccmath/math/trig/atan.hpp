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

#include "ccmath/internal/math/generic/builtins/trig/atan.hpp"
#include "ccmath/internal/math/runtime/func/trig/atan_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/trig/impl/inv_trig_impl.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Computes the principal value of the inverse tangent in radians.
	 * @tparam T Floating-point type.
	 * @param num Floating-point value.
	 * @return Inverse tangent of num in radians.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true> constexpr T atan(T num)
	{
		if constexpr (ccm::builtin::has_constexpr_atan<T>)
		{
			return ccm::builtin::atan_ct(num);
		} else if (ccm::support::is_constant_evaluated())
		{
			if constexpr (std::is_same_v<T, float>)
			{
				return internal::impl::atan_float(num);
			} else if constexpr (std::is_same_v<T, double>)
			{
				return internal::impl::atan_double(num);
			} else
			{
				return static_cast<long double>(internal::impl::atan_double(static_cast<double>(num)));
			}
		} else
		{
			return ccm::rt::atan_rt(num);
		}
	}

	/**
	 * @brief Computes inverse tangent of an integer input after promotion to double.
	 * @tparam Integer Integral type.
	 * @param num Input value.
	 * @return Inverse tangent in radians as double.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true> constexpr double atan(Integer num)
	{
		return ccm::atan<double>(static_cast<double>(num));
	}

	/**
	 * @brief Computes inverse tangent for float.
	 * @param num Input value.
	 * @return Inverse tangent in radians as float.
	 */
	constexpr float atanf(float num)
	{
		return ccm::atan<float>(num);
	}

	/**
	 * @brief Computes inverse tangent for long double.
	 * @param num Input value.
	 * @return Inverse tangent in radians as long double.
	 */
	constexpr long double atanl(long double num)
	{
		return ccm::atan<long double>(num);
	}
} // namespace ccm

/// @ingroup trig
