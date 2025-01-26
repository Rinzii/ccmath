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

#include "ccmath/internal/math/generic/builtins/fmanip/copysign.hpp"
#include "ccmath/math/basic/fabs.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/math/compare/signbit.hpp"

namespace ccm
{
	/**
	 * @brief Copies the sign of a floating point or integer value.
	 * @tparam T Type of the floating-point or integer value.
	 * @param mag A floating-point or integer value
	 * @param sgn A floating-point or integer value
	 * @return If no errors occur, the floating point value with the magnitude of mag and the sign of sgn is returned.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T copysign(T mag, T sgn)
	{
		if constexpr (ccm::builtin::has_constexpr_copysign<T>) { return ccm::builtin::copysign(mag, sgn); }
		else
		{
			if (ccm::isnan(mag) || ccm::isnan(sgn))
			{
				if (ccm::signbit(sgn)) { return -std::numeric_limits<T>::quiet_NaN(); }
				return std::numeric_limits<T>::quiet_NaN();
			}

			T sign_bit = ccm::signbit(sgn) ? T(-1) : T(1);
			return ccm::abs(mag) * sign_bit;
		}
	}

	/**
	 * @brief Copies the sign of an integer value.
	 * @tparam Integer Type of the integer value.
	 * @param mag A integer value
	 * @param sgn A integer value
	 * @return If no errors occur, the floating point value with the magnitude of mag and the sign of sgn is returned.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double copysign(Integer mag, Integer sgn)
	{
		return ccm::copysign<double>(static_cast<double>(mag), static_cast<double>(sgn));
	}

	/**
	 * @brief Copies the sign of a floating point value.
	 * @param mag A floating-point.
	 * @param sgn A floating-point.
	 * @return If no errors occur, the floating point value with the magnitude of mag and the sign of sgn is returned.
	 */
	constexpr float copysignf(float mag, float sgn)
	{
		return ccm::copysign<float>(mag, sgn);
	}

	/**
	 * @brief Copies the sign of a floating point value.
	 * @param mag A floating-point.
	 * @param sgn A floating-point.
	 * @return If no errors occur, the floating point value with the magnitude of mag and the sign of sgn is returned.
	 */
	constexpr long double copysignl(long double mag, long double sgn)
	{
		return ccm::copysign<long double>(mag, sgn);
	}
} // namespace ccm
