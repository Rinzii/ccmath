/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

/**
 * @file ldexp.hpp
 * @defgroup fmanip Floating-Point Manipulation
 * @brief Provides functions for scaling floating-point (and integer) values by a power of two.
 *
 * This header defines a family of ldexp-like functions, including \c ldexp for
 * generic floating-point types, integer overloads, and specialized variants for
 * \c float and \c long double. They enable multiplication by \f$2^\text{exp}\f$
 * while handling overflow, underflow, and special values (NaN, ±∞, etc.) as per
 * the rules of floating-point arithmetic so long as errno has not been disabled in cmake.
 *
 * @author
 *   Ian Pike
 *
 * @copyright
 *   Copyright (c) Ian Pike
 *   Copyright (c) CCMath contributors
 *   Licensed under the Apache-2.0 License WITH LLVM-exception.
 */

// Resharper gets confused and thinks there is an unreachable code
// ReSharper disable CppDFAUnreachableFunctionCall

#pragma once

#include "ccmath/internal/config/builtin/ldexp_support.hpp"
#include "ccmath/internal/math/generic/builtins/fmanip/ldexp.hpp"
#include "ccmath/internal/support/helpers/internal_ldexp.hpp"

/**
 * @brief global ccm namespace
 */
namespace ccm
{
	/**
	 * @brief Scales a floating-point \p num by \f$2^\text{exp}\f$.
	 *
	 * Overflow returns ±HUGE_VAL (or its float and long double forms), underflow returns the
	 * correctly rounded result, possibly a subnormal or zero. ±0, ±∞, and NaN pass through
	 * unchanged, and \p num is returned as-is when \p exp is 0. On a range error, and unless
	 * errno was disabled in cmake, \c errno may be set to \c ERANGE and \c FE_OVERFLOW or
	 * \c FE_UNDERFLOW raised, depending on the platform.
	 *
	 * @tparam T Floating-point type.
	 * @param num Value to scale.
	 * @param exp Power of two to scale by.
	 * @return \f$\text{num} \times 2^\text{exp}\f$.
	 * @attention At compile time errno may or may not be set, depending on the compiler. Assume it is not.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true> constexpr T ldexp(T num, int exp) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_ldexp<T>)
		{
			return builtin::ldexp_ct(num, exp);
		} else
		{
			return support::helpers::internal_ldexp(num, exp);
		}
	}

	/**
	 * @brief Scales an integer \p num by \f$2^\text{exp}\f$, converting it to \c double first.
	 *
	 * Equivalent to \c ldexp<double>(static_cast<double>(num), exp), so it shares the
	 * floating-point overload's behavior after the conversion. Overflow returns ±HUGE_VAL.
	 *
	 * @tparam Integer Integer type.
	 * @param num Value to scale, converted to \c double.
	 * @param exp Power of two to scale by.
	 * @return \f$\text{num} \times 2^\text{exp}\f$ as a \c double.
	 * @attention At compile time errno may or may not be set, depending on the compiler. Assume it is not.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true> constexpr double ldexp(Integer num, int exp) noexcept
	{
		return ccm::ldexp<double>(static_cast<double>(num), exp);
	}

	/**
	 * @brief Scales a \c float \p num by \f$2^\text{exp}\f$. Equivalent to \c ldexp<float>(num, exp).
	 *
	 * Overflow returns ±HUGE_VALF, underflow a subnormal or zero.
	 *
	 * @param num Value to scale.
	 * @param exp Power of two to scale by.
	 * @return \f$\text{num} \times 2^\text{exp}\f$ as a \c float.
	 * @attention At compile time errno may or may not be set, depending on the compiler. Assume it is not.
	 */
	constexpr float ldexpf(float num, int exp) noexcept
	{
		return ccm::ldexp<float>(num, exp);
	}

	/**
	 * @brief Scales a \c long double \p num by \f$2^\text{exp}\f$ using native long double support.
	 *
	 * Overflow returns ±HUGE_VALL, underflow a subnormal or zero. ±0, ±∞, and NaN pass through
	 * unchanged, and \p num is returned as-is when \p exp is 0. On a range error, and unless errno
	 * was disabled in cmake, \c errno may be set to \c ERANGE and \c FE_OVERFLOW or \c FE_UNDERFLOW
	 * raised, depending on the platform.
	 *
	 * @param num Value to scale.
	 * @param exp Power of two to scale by.
	 * @return \f$\text{num} \times 2^\text{exp}\f$ as a \c long double.
	 * @attention At compile time errno may or may not be set, depending on the compiler. Assume it is not.
	 */
	constexpr long double ldexpl(long double num, int exp) noexcept
	{
		return ccm::ldexp<long double>(num, exp);
	}

} // namespace ccm
