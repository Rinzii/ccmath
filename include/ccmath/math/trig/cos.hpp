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

#include "ccmath/internal/math/generic/builtins/trig/cos.hpp"
#include "ccmath/internal/math/generic/func/trig/cos_gen.hpp"
#include "ccmath/internal/math/runtime/func/trig/cos_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Computes the cosine of an angle in radians.
	 * @tparam T Floating-point type.
	 * @param num Angle in radians.
	 * @return Cosine of num.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T cos(T num)
	{
		if constexpr (ccm::builtin::has_constexpr_cos<T>) { return ccm::builtin::cos_ct(num); }
		else
		{
			if (ccm::support::is_constant_evaluated()) { return ccm::gen::cos_gen(num); }
			return ccm::rt::cos_rt(num);
		}
	}

	/**
	 * @brief Computes the cosine of an integral angle value after promotion to double.
	 * @tparam Integer Integral type.
	 * @param num Angle in radians.
	 * @return Cosine of num as double.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double cos(Integer num)
	{ return ccm::cos<double>(static_cast<double>(num)); }

	/**
	 * @brief Computes the cosine of a float angle in radians.
	 * @param num Angle in radians.
	 * @return Cosine as float.
	 */
	constexpr float cosf(float num)
	{ return ccm::cos<float>(num); }

	/**
	 * @brief Computes the cosine of a long double angle in radians.
	 * @param num Angle in radians.
	 * @return Cosine as long double.
	 */
	constexpr long double cosl(long double num)
	{ return ccm::cos<long double>(num); }
} // namespace ccm

/// @ingroup trig
