/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <ccmath/internal/support/bits.hpp>
#include "ccmath/internal/support/floating_point_traits.hpp"

namespace ccm
{
	/**
	 * @brief Multiplies a floating point value num by the number 2 raised to the exp power.
	 * @note On many implementations, std::ldexp is less efficient than multiplication or division by a power of two using arithmetic operators.
	 * @tparam num A floating-point type.
	 * @param exp An integer value.
	 * @return If no errors occur, num multiplied by 2 to the power of exp (num×2exp) is returned.\nIf a range error due to overflow occurs, ±HUGE_VAL, ±HUGE_VALF, or ±HUGE_VALL is returned.\nIf a range error due to underflow occurs, the correct result (after rounding) is returned.
	 */
	template <typename Floating_Point, std::enable_if_t<!std::is_integral_v<Floating_Point>, bool> = true>
	inline constexpr Floating_Point ldexp(Floating_Point num, int32_t exp) noexcept
	{
		int32_t oldexp = ccm::support::get_exponent_of_floating_point<Floating_Point>(num);
		
		// if the mantissa is 0 and the original exponent is 0, or infinite, return num
		if (CCM_UNLIKELY(!ccm::isfinite(num)) || 
			((oldexp == 0) && 
				((ccm::support::bit_cast<ccm::helpers::float_bits_t<Floating_Point>>(num) 
				 & ccm::helpers::floating_point_traits<Floating_Point>::normal_mantissa_mask) == 0)))
		{
			return num;
		}

		if (exp > ccm::helpers::floating_point_traits<Floating_Point>::maximum_binary_exponent)
		{
			// error == hugeval
			return std::numeric_limits<Floating_Point>::infinity();
		}
		// the reference source says -2 * exp_max
		else if (exp < ccm::helpers::floating_point_traits<Floating_Point>::minimum_binary_exponent)
		{
			// error == range
			return 0;
		}
		// normalizes an abnormal floating point
		if (oldexp == 0)
		{
			num *= ccm::helpers::floating_point_traits<Floating_Point>::normalize_factor;
			exp	   = -sizeof(Floating_Point) * std::numeric_limits<unsigned char>::digits; // bits in a byte
			oldexp = ccm::support::get_exponent_of_floating_point<Floating_Point>(num);
		}

		exp += oldexp;
		if (exp >= ccm::helpers::floating_point_traits<Floating_Point>::maximum_binary_exponent)
		{
			// overflow
			return std::numeric_limits<Floating_Point>::infinity();
		}
		if (exp > 0) { 
			return ccm::support::set_exponent_of_floating_point<Floating_Point>(num, exp);
		}
		// denormal, or underflow
		exp += sizeof(Floating_Point) * std::numeric_limits<unsigned char>::digits; // bits in a byte
		num = ccm::support::set_exponent_of_floating_point<Floating_Point>(num, exp);
		num /= ccm::helpers::floating_point_traits<Floating_Point>::normalize_factor;
		//if (num == static_cast<Floating_Point>(0))
		//{
			// underflow report not currently being handled
		//}
		return num;
	}

	/**
	 * @brief Multiplies a floating point value num by the number 2 raised to the exp power.
	 * @note On many implementations, std::ldexp is less efficient than multiplication or division by a power of two using arithmetic operators.
	 * @tparam num An integer type.
	 * @param exp An integer value.
	 * @return If no errors occur, num multiplied by 2 to the power of exp (num×2exp) is returned.\nIf a range error due to overflow occurs, ±HUGE_VAL,
	 * ±HUGE_VALF, or ±HUGE_VALL is returned.\nIf a range error due to underflow occurs, the correct result (after rounding) is returned.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	inline constexpr double ldexp(Integer num, int32_t exp) noexcept
	{
		return ldexp<double>(static_cast<double>(num), exp);
	}
	/**
	 * @brief Returns the floating-point remainder of the division operation x/y.
	 * @note
	 * @tparam num A floating-point type.
	 * @param exp An integer value.
	 * @return The result of parameter num multiplied by 2 to the power of parameter exp.
	 */
	inline constexpr float ldexpf(float num, int32_t exp) noexcept
	{
		return ldexp<float>(num, exp);
	}
	/**
	 * @brief Returns the floating-point remainder of the division operation x/y.
	 * @note
	 * @tparam num A floating-point type.
	 * @param exp An integer value.
	 * @return The result of parameter num multiplied by 2 to the power of parameter exp.
	 */
	inline constexpr long double ldexpl(long double num, int exp) noexcept
	{
		assert(false && "long double support isn't implemented");
		return ldexp<long double>(num, exp);
	}

} // namespace ccm
