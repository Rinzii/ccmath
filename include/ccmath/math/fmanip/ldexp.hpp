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
	 * @brief Multiplies a floating-point or integer value \p num by 2 raised to the power \p exp.
	 *
	 * Additional overloads accept an integer \p num and treat it as double.
	 *
	 * <b>Parameters</b>
	 * - \p num — Floating-point or integer value to be scaled.
	 * - \p exp — Integer exponent by which \p num is multiplied (power of two).
	 *
	 * <b>Return value</b>
	 * - If no errors occur, returns the product \f$\text{num} \times 2^\text{exp}\f$.
	 * - If a range error due to overflow occurs, ±HUGE_VAL, ±HUGE_VALF, or ±HUGE_VALL is returned.
	 * - If a range error due to underflow occurs, the correct result (after rounding) is returned,
	 *   which may be 0 or a denormalized value.
	 *
	 * <b>Error handling</b>
	 * - Errors are reported according to the platform’s math error-handling rules and if errno has not been disabled.
	 * - If range errors occur, \c errno may be set to \c ERANGE and floating-point exceptions such
	 *   as \c FE_OVERFLOW or \c FE_UNDERFLOW may be raised, depending on the implementation.
	 * - If \p num is ±0 or ±∞, the function returns it unmodified.
	 * - If \p exp is 0, \p num is returned unmodified.
	 * - If \p num is NaN, NaN is returned.
	 *
	 * <b>Notes</b>
	 * - On binary systems where FLT_RADIX is 2, this function can be viewed as a specialized way
	 *   of scaling \p num by a power of two.
	 * - Using arithmetic operators with powers of two may be faster on some platforms, but
	 *   this function ensures a clear, consistent interface for exponent shifting.
	 * - Overloads are available for all integer types, converting \p num to \c double first.
	 *
	 * <b>Example</b>
	 * \code{.cpp}
	 * #include <cstdio>
	 * #include <ccmath/math/basic/ldexp.hpp>
	 *
	 * int main() {
	 *     double val = 1.75;
	 *     int expVal = 3;
	 *     double result = ccm::ldexp(val, expVal);
	 *     std::printf("ldexp(%f, %d) = %f\n", val, expVal, result);
	 *
	 *     int i = 5;
	 *     double resultInt = ccm::ldexp(i, 2);
	 *     std::printf("ldexp(%d, %d) = %f\n", i, 2, resultInt);
	 *     return 0;
	 * }
	 * \endcode
	 *
	 * @attention During compile time evaluation, errno may or may not be set depending on the compiler. Generally assume it is not.
	 *
	 * @see https://en.cppreference.com/w/cpp/numeric/math/ldexp
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T ldexp(T num, int exp)
	{
		if constexpr (ccm::builtin::has_constexpr_ldexp<T>) { return builtin::ldexp(num, exp); }
		else { return support::helpers::internal_ldexp(num, exp); }
	}

	/**
	 * @brief Overload for integer \p num, multiplying it by 2 rose to the power \p exp.
	 *
	 * The integer \p num is converted to \c double before the multiplication, and the result
	 * is returned as a \c double.
	 *
	 * <b>Parameters</b>
	 * - \p num — Integer value to be scaled (cast to \c double internally).
	 * - \p exp — Integer exponent by which \p num is multiplied (power of two).
	 *
	 * <b>Return value</b>
	 * - If no errors occur, returns the product `num * 2^exp` as a \c double.
	 * - If a range error due to overflow occurs, ±HUGE_VAL is returned.
	 * - If a range error due to underflow occurs, the correct result (after rounding) is returned,
	 *   which may be 0 or a denormalized value.
	 *
	 * <b>Error handling</b>
	 * - Same as the floating-point overload. If errors occur, \c errno may be set to \c ERANGE and
	 *   floating-point exceptions such as \c FE_OVERFLOW or \c FE_UNDERFLOW may be raised if errno has not been disabled.
	 *
	 * <b>Notes</b>
	 * - This overload is equivalent to calling \c ldexp<double>(static_cast<double>(num), exp).
	 *
	 * @attention During compile time evaluation, errno may or may not be set depending on the compiler. Generally assume it is not.
	 *
	 * @see https://en.cppreference.com/w/cpp/numeric/math/ldexp
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double ldexp(Integer num, int exp)
	{
		return ccm::ldexp<double>(static_cast<double>(num), exp);
	}

	/**
	 * @brief Multiplies a float \p num by 2 raised to the power \p exp.
	 *
	 * Equivalent to calling \c ldexp<float>(num, exp).
	 *
	 * <b>Parameters</b>
	 * - \p num — Floating-point (float) value to be scaled.
	 * - \p exp — Integer exponent by which \p num is multiplied (power of two).
	 *
	 * <b>Return value</b>
	 * - The product \f$\text{num} \times 2^\text{exp}\f$ as a float.
	 * - May return ±HUGE_VALF on overflow, or a denormalized value or 0 on underflow.
	 *
	 * <b>Error handling</b>
	 * - As with the other overloads, \c errno may be set to \c ERANGE and floating-point exceptions
	 *   (e.g., \c FE_OVERFLOW, \c FE_UNDERFLOW) may occur depending on the platform if errno has not been disabled.
	 *
	 * @attention During compile time evaluation, errno may or may not be set depending on the compiler. Generally assume it is not.

	 * @see https://en.cppreference.com/w/cpp/numeric/math/ldexp
	 */
	constexpr float ldexpf(float num, int exp) noexcept
	{
		return ccm::ldexp<float>(num, exp);
	}

	/**
	 * @brief Multiplies a long double \p num by 2 raised to the power \p exp.
	 *
	 * <b>Parameters</b>
	 * - \p num — A \c long double value to be scaled by a power of two.
	 * - \p exp — An integer exponent by which \p num is multiplied (i.e., \f$2^\text{exp}\f$).
	 *
	 * <b>Return value</b>
	 * - The product \f$\text{num} \times 2^\text{exp}\f$ as a \c long double.
	 * - May return ±HUGE_VALL on overflow, or a denormalized value or 0 on underflow.
	 *
	 * <b>Error handling</b>
	 * - If range errors occur, \c errno may be set to \c ERANGE, and floating-point exceptions
	 *   (\c FE_OVERFLOW, \c FE_UNDERFLOW) may be raised, depending on the implementation and if errno has not been disabled.
	 * - If \p num is ±0 or ±∞, the function returns it unmodified.
	 * - If \p exp is 0, \p num is returned unmodified.
	 * - If \p num is NaN, NaN is returned.
	 *
	 * <b>Notes</b>
	 * - This overload uses native \c long double support for multiplying by a power of two.
	 *
	 * @attention During compile time evaluation, errno may or may not be set depending on the compiler. Generally assume it is not.
	 *
	 * @see https://en.cppreference.com/w/cpp/numeric/math/ldexp
	 */
	constexpr long double ldexpl(long double num, int exp) noexcept
	{
		return ccm::ldexp<long double>(num, exp);
	}

} // namespace ccm
