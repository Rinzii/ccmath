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

#include "ccmath/internal/config/builtin/bit_cast_support.hpp"
#include "ccmath/internal/config/builtin/ldexp_support.hpp"
#include "ccmath/internal/math/generic/builtins/fmanip/ldexp.hpp"
#include "ccmath/internal/predef/has_const_builtin.hpp"
#include "ccmath/internal/support/helpers/internal_ldexp.hpp"


namespace ccm
{
	/**
	 * @brief Multiplies a floating point value num by the number 2 raised to the exp power.
	 * @note On many implementations, std::ldexp is less efficient than multiplication or division by a power of two using arithmetic operators.
	 * @tparam T A floating-point type or an integer type.
	 * @param num A floating-point type or an integer value.
	 * @param exp An integer value.
	 * @return If no errors occur, num multiplied by 2 to the power of exp (num×2exp) is returned.\nIf a range error due to overflow occurs, ±HUGE_VAL,
	 * ±HUGE_VALF, or ±HUGE_VALL is returned.\nIf a range error due to underflow occurs, the correct result (after rounding) is returned.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T ldexp(T num, int exp) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_ldexp<T>) { return ccm::builtin::ldexp(num, exp); }
		else { return support::helpers::internal_ldexp(num, exp); }
	}

	/**
	 * @brief Multiplies a floating point value num by the number 2 raised to the exp power.
	 * @note On many implementations, std::ldexp is less efficient than multiplication or division by a power of two using arithmetic operators.
	 * @tparam Integer An integer type
	 * @param num An integer value.
	 * @param exp An integer value.
	 * @return If no errors occur, num multiplied by 2 to the power of exp (num×2exp) is returned.\nIf a range error due to overflow occurs, ±HUGE_VAL,
	 * ±HUGE_VALF, or ±HUGE_VALL is returned.\nIf a range error due to underflow occurs, the correct result (after rounding) is returned.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double ldexp(Integer num, int exp) noexcept
	{
		return ccm::ldexp<double>(static_cast<double>(num), exp);
	}
	/**
	 * @brief Returns the floating-point remainder of the division operation x/y.
	 * @note
	 * @param num A float value.
	 * @param exp An integer value.
	 * @return The result of parameter num multiplied by 2 to the power of parameter exp.
	 */
	constexpr float ldexpf(float num, int exp) noexcept
	{
		return ccm::ldexp<float>(num, exp);
	}
	/**
	 * @brief Returns the floating-point remainder of the division operation x/y.
	 * @note
	 * @param num A long double value.
	 * @param exp An integer value.
	 * @return The result of parameter num multiplied by 2 to the power of parameter exp.
	 */
	constexpr long double ldexpl(long double num, int exp) noexcept
	{
		// long double isn't supported yet
		return ccm::ldexp<double>(static_cast<double>(num), exp);
	}

} // namespace ccm
