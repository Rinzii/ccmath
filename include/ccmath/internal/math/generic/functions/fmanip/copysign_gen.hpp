/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/basic/abs.hpp"
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
