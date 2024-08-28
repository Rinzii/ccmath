/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/builtin/bit_cast_support.hpp"
#include "ccmath/internal/config/builtin/ldexp_support.hpp"
#include "ccmath/internal/helpers/internal_ldexp.hpp"
#include "ccmath/internal/predef/has_const_builtin.hpp"

/* TODO: Move, remove, or change this to not use bit_cast.
	#include "ccmath/internal/support/bits.hpp"
	#include "ccmath/internal/support/fenv/fenv_support.hpp"
	#include "ccmath/internal/support/floating_point_traits.hpp"
	#include "ccmath/math/compare/isfinite.hpp"

	#include <limits>
*/

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
#if defined(CCMATH_HAS_CONSTEXPR_BUILTIN_LDEXP) || CCM_HAS_CONST_BUILTIN(__builtin_ldexp)
		if constexpr (std::is_same_v<T, float>) { return __builtin_ldexpf(num, exp); }
		if constexpr (std::is_same_v<T, double>) { return __builtin_ldexp(num, exp); }
		if constexpr (std::is_same_v<T, long double>) { return __builtin_ldexpl(num, exp); }
		return static_cast<T>(__builtin_ldexpl(num, exp));
#else
		return helpers::internal_ldexp(num, exp);
		/* TODO: Move, remove, or change this to not use bit_cast.
		// Fallback option. Does not give perfect results, but generally good enough.
		int old_exp = static_cast<int>(support::get_exponent_of_floating_point<T>(num));

		// if the mantissa is 0 and the original exponent is 0, or infinite, return num

		if (const auto bits = support::bit_cast<support::float_bits_t<T>>(num);
			CCM_UNLIKELY(!ccm::isfinite(num)) || old_exp < support::floating_point_traits<T>::minimum_binary_exponent ||
			((old_exp == 0) && ((bits & support::floating_point_traits<T>::normal_mantissa_mask) == 0)))
		{
			return num;
		}

		// An overflow will occur if the exponent is larger than the maximum exponent
		if (exp > support::floating_point_traits<T>::maximum_binary_exponent)
		{
			// These functions do nothing at compile time, but at runtime will set errno and raise exceptions if required.
			support::fenv::set_errno_if_required(ERANGE);
			support::fenv::raise_except_if_required(FE_OVERFLOW);

			return std::numeric_limits<T>::infinity();
		}
		// the reference source says -2 * exp_max
		// An underflow has occurred if the exponent is less than the minimum exponent
		if (exp < support::floating_point_traits<T>::minimum_binary_exponent)
		{
			// These functions do nothing at compile time, but at runtime will set errno and raise exceptions if required.
			support::fenv::set_errno_if_required(ERANGE);
			support::fenv::raise_except_if_required(FE_UNDERFLOW);

			return 0;
		}
		// normalizes an abnormal floating point
		if (old_exp == 0)
		{
			num *= support::floating_point_traits<T>::normalize_factor;
			exp		= -static_cast<std::int32_t>(sizeof(T)) * std::numeric_limits<unsigned char>::digits; // bits in a byte
			old_exp = static_cast<int>(support::get_exponent_of_floating_point<T>(num));
		}

		exp += old_exp;
		// If we left maximum exponent, we need to return infinity and report overflow
		if (exp >= support::floating_point_traits<T>::maximum_binary_exponent)
		{
			// These functions do nothing at compile time, but at runtime will set errno and raise exceptions if required.
			support::fenv::set_errno_if_required(ERANGE);
			support::fenv::raise_except_if_required(FE_OVERFLOW);

			return std::numeric_limits<T>::infinity();
		}
		if (exp > 0) { return static_cast<int>(support::set_exponent_of_floating_point<T>(num, exp)); }
		// denormal, or underflow
		exp += static_cast<std::int32_t>(sizeof(T)) * std::numeric_limits<unsigned char>::digits; // bits in a byte
		num = support::set_exponent_of_floating_point<T>(num, exp);
		num /= support::floating_point_traits<T>::normalize_factor;

		return num;
		 */
#endif
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
