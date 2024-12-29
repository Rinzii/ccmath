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

#include "ccmath/math/basic/fabs.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/math/compare/signbit.hpp"

namespace ccm
{
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T copysign(T mag, T sgn)
	{
		if (ccm::isnan(mag) || ccm::isnan(sgn))
		{
			if (ccm::signbit(sgn)) { return -std::numeric_limits<T>::quiet_NaN(); }
			return std::numeric_limits<T>::quiet_NaN();
		}

		T sign_bit = ccm::signbit(sgn) ? -1 : 1;
		return ccm::abs(mag) * sign_bit;
	}
} // namespace ccm
