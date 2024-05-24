/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <ccmath/math/compare/isfinite.hpp>
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/floating_point_traits.hpp"

#include <limits>

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
		ccm::support::float_signed_bits_t<T> old_exp = ccm::support::get_exponent_of_floating_point<T>(num);

		// if the mantissa is 0 and the original exponent is 0, or infinite, return num
		const auto bits = ccm::support::bit_cast<ccm::support::float_bits_t<T>>(num);

		if (CCM_UNLIKELY(!ccm::isfinite(num)) || old_exp < ccm::support::floating_point_traits<T>::minimum_binary_exponent ||
			((old_exp == 0) && ((bits & ccm::support::floating_point_traits<T>::normal_mantissa_mask) == 0)))
		{
			return num;
		}

		if (exp > ccm::support::floating_point_traits<T>::maximum_binary_exponent)
		{
			// error == hugeval
			return std::numeric_limits<T>::infinity();
		}
		// the reference source says -2 * exp_max
		if (exp < ccm::support::floating_point_traits<T>::minimum_binary_exponent)
		{
			// error == range
			return 0;
		}
		// normalizes an abnormal floating point
		if (old_exp == 0)
		{
			num *= ccm::support::floating_point_traits<T>::normalize_factor;
			exp		= -static_cast<std::int32_t>(sizeof(T)) * std::numeric_limits<unsigned char>::digits; // bits in a byte
			old_exp = ccm::support::get_exponent_of_floating_point<T>(num);
		}

		exp += old_exp;
		if (exp >= ccm::support::floating_point_traits<T>::maximum_binary_exponent)
		{
			// overflow
			return std::numeric_limits<T>::infinity();
		}
		if (exp > 0) { return ccm::support::set_exponent_of_floating_point<T>(num, exp); }
		// denormal, or underflow
		exp += static_cast<std::int32_t>(sizeof(T)) * std::numeric_limits<unsigned char>::digits; // bits in a byte
		num = ccm::support::set_exponent_of_floating_point<T>(num, exp);
		num /= ccm::support::floating_point_traits<T>::normalize_factor;
		// if (num == static_cast<T>(0))
		//{
		//  underflow report not currently being handled
		//}
		return num;
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
		return ldexp<double>(static_cast<double>(num), exp);
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
		return ldexp<float>(num, exp);
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
		return ldexp<double>(static_cast<double>(num), exp);
	}

} // namespace ccm
