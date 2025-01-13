/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// Portions of this code was borrowed from the LLVM Project,
// under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

/**
 * @internal
 *
 * @file dyadic_float.hpp
 * @defgroup types
 * @brief Internal high-precision dyadic floating-point type for CCMath.
 *
 * This file contains the definition and implementation details for a high-precision
 * floating-point data type in “dyadic” form.
 * It is used internally by CCMath for dyadic floating-point computations
 * that don't fit inside the standard floating-point types.
 *
 * @see ccmath/internal/support/fp/fp_bits.hpp
 * @see ccmath/internal/support/type_traits.hpp
 * @see ccmath/internal/types/big_int.hpp
 */

#pragma once

#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/support/multiply_add.hpp"
#include "ccmath/internal/support/type_traits.hpp"
#include "ccmath/internal/types/big_int.hpp"

#include <cstddef>

namespace ccm::types
{

	/**
	 * @internal
	 * @ingroup types
	 *
	 * @brief A class for high-precision floating point computations using a dyadic format.
	 *
	 * This class represents high-precision floating-point values in a dyadic format, consisting of three fields:
	 * - <b>sign</b>: A boolean value where `false` indicates positive and `true` indicates negative.
	 * - <b>exponent</b>: The exponent value (for the least significant bit of the mantissa).
	 * - <b>mantissa</b>: An unsigned integer of length `Bits`.
	 *
	 * The stored value is calculated as:
	 * @code
	 * value = (-1)^sign * 2^exponent * (mantissa_as_unsigned_integer)
	 * @endcode
	 *
	 * The data is considered <em>normalized</em> if, for a non-zero mantissa, the leading bit is 1.
	 * Constructors and most member functions ensure that their outputs are normalized.
	 * To simplify and improve efficiency, many functions assume that the inputs are already normalized.
	 *
	 * @note <b>Edge Cases: </b>
	 *  - <b>Zero mantissa</b>: A zero mantissa represents a signed zero (\c +0 or \c -0).
	 *  - <b>Large exponents</b>: Extremely large exponents can lead to overflow behavior if converted to a standard
	 *    floating-point type, potentially resulting in infinity.
	 *  - <b>Small exponents</b>: Very small (negative) exponents can produce underflow or subnormal values when converted.
	 *    Depending on rounding mode and whether exceptions are signaled, this can raise \c FE_UNDERFLOW or set \c errno.
	 *
	 * @tparam Bits The length of the mantissa in bits.
	 */
	template <size_t Bits>
	struct DyadicFloat
	{
		using mantissa_type = UInt<Bits>;

		Sign sign			   = Sign::POS;
		int exponent		   = 0;
		mantissa_type mantissa = mantissa_type(0);

		constexpr DyadicFloat() = default;

		template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
		constexpr DyadicFloat(T x)
		{
			static_assert(support::fp::FPBits<T>::fraction_length < Bits);
			support::fp::FPBits<T> x_bits(x);
			sign	 = x_bits.sign();
			exponent = x_bits.get_explicit_exponent() - support::fp::FPBits<T>::fraction_length;
			mantissa = mantissa_type(x_bits.get_explicit_mantissa());
			normalize();
		}

		constexpr DyadicFloat(Sign s, int e, mantissa_type m) : sign(s), exponent(e), mantissa(m) { normalize(); }

		/**
		 * @brief Normalizes the mantissa, bringing the leading 1 bit to the most significant bit.
		 *
		 * This function shifts the mantissa so that the leading 1 bit is in the most significant bit position.
		 * It adjusts the exponent accordingly.
		 *
		 * @return A reference to the normalized DyadicFloat.
		 */
		constexpr DyadicFloat & normalize()
		{
			if (!mantissa.is_zero())
			{
				const int shift_length = support::countl_zero(mantissa);
				exponent -= shift_length;
				mantissa <<= static_cast<std::size_t>(shift_length);
			}
			return *this;
		}

		/**
		 * @brief Shifts the mantissa left to align exponents. Output might not be normalized.
		 *
		 * This function shifts the mantissa left by the specified number of bits to help align exponents.
		 * The exponent is adjusted accordingly. The result may not be normalized.
		 *
		 * @param shift_length The number of bits to shift left.
		 * @return A reference to the DyadicFloat after shifting.
		 */
		constexpr DyadicFloat & shift_left(unsigned shift_length)
		{
			if (shift_length < Bits)
			{
				exponent -= static_cast<int>(shift_length);
				mantissa <<= shift_length;
			}
			else
			{
				exponent = 0;
				mantissa = mantissa_type(0);
			}
			return *this;
		}

		/**
		 * @brief Shifts the mantissa right to align exponents. Output might not be normalized.
		 *
		 * This function shifts the mantissa right by the specified number of bits to help align exponents.
		 * The exponent is adjusted accordingly. The result may not be normalized.
		 *
		 * @param shift_length The number of bits to shift right.
		 * @return A reference to the DyadicFloat after shifting.
		 */
		constexpr DyadicFloat & shift_right(unsigned shift_length)
		{
			if (shift_length < Bits)
			{
				exponent += static_cast<int>(shift_length);
				mantissa >>= shift_length;
			}
			else
			{
				exponent = 0;
				mantissa = mantissa_type(0);
			}
			return *this;
		}

		/**
		 * @brief Returns the unbiased exponent, assuming normalization.
		 *
		 * @return The unbiased exponent.
		 */
		[[nodiscard]] constexpr int get_unbiased_exponent() const { return exponent + (Bits - 1); }

		/**
		 * @brief Converts this DyadicFloat to the specified floating-point type \p T.
		 *
		 * This function treats the current DyadicFloat (with sign, exponent, and mantissa)
		 * as a normalized number and constructs a floating-point value of type \p T.
		 * Rounding is handled according to the active floating-point environment.
		 * When \p ShouldSignalExceptions is true, certain edge cases (overflow, underflow)
		 * may raise floating-point exceptions (e.g., FE_OVERFLOW, FE_UNDERFLOW) or set \c errno.
		 *
		 * - If the mantissa is zero, the function returns \c +0 or \c -0 depending on the sign.
		 * - If the exponent is so large that even normalized representation overflows \p T,
		 *   the function returns infinity (and may raise FE_OVERFLOW).
		 * - If the exponent is too small (resulting in denormalized \p T), it clears the implicit bit,
		 *   resulting in a subnormal value. This may raise FE_UNDERFLOW if \p ShouldSignalExceptions
		 *   is set.
		 *
		 * @tparam T A floating-point type (e.g., float, double) that satisfies
		 *           \code std::is_floating_point_v<T> && (support::fp::FPBits<T>::fraction_length < Bits) \endcode.
		 * @tparam ShouldSignalExceptions If true, overflow and underflow conditions may
		 *                                raise floating-point exceptions or set \c errno.
		 * @return A value of type \p T, which is the rounded result of converting
		 *         this DyadicFloat to \p T.
		 */

		template <typename T, bool ShouldSignalExceptions,
				  typename = std::enable_if_t<std::is_floating_point_v<T> && (support::fp::FPBits<T>::fraction_length < Bits), void>>
		constexpr T fast_as() const
		{
			if (CCM_UNLIKELY(mantissa.is_zero())) { return support::fp::FPBits<T>::zero(sign).get_val(); }

			// Assume that it is normalized, and output is also normal.
			constexpr std::uint32_t precision	  = support::fp::FPBits<T>::fraction_length + 1;
			using output_bits_t					  = typename support::fp::FPBits<T>::storage_type;
			constexpr output_bits_t implicit_mask = support::fp::FPBits<T>::significand_mask - support::fp::FPBits<T>::fraction_mask;

			int exp_hi = exponent + static_cast<int>((Bits - 1) + support::fp::FPBits<T>::exponent_bias);

			if (CCM_UNLIKELY(exp_hi > 2 * support::fp::FPBits<T>::exponent_bias))
			{
				// Results overflow.
				T d_hi = support::fp::FPBits<T>::create_value(sign, 2 * support::fp::FPBits<T>::exponent_bias, implicit_mask).get_val();

				// volatile prevents constant propagation that would result in infinity
				// always being returned no matter the current rounding mode.
				volatile T two = static_cast<T>(2.0);
				T r			   = two * d_hi;

				// TODO: Whether rounding down the absolute value to max_normal should
				//		also raise FE_OVERFLOW and set ERANGE is debatable.
				// TODO: Check what MSVC does for this case to match them when compiling with MSVC.
				// This behavior matches Clang.
				if constexpr (ShouldSignalExceptions && support::fp::FPBits<T>(r).is_inf()) { support::fenv::set_errno_if_required(ERANGE); }

				return r;
			}

			bool denorm			= false;
			std::uint32_t shift = Bits - precision;
			if (CCM_UNLIKELY(exp_hi <= 0))
			{
				// Output is denormal.
				denorm = true;
				shift  = (Bits - precision) + static_cast<std::uint32_t>(1 - exp_hi);

				exp_hi = support::fp::FPBits<T>::exponent_bias;
			}

			int exp_lo = exp_hi - static_cast<int>(precision) - 1;

			mantissa_type m_hi = shift >= mantissa_type::BITS ? mantissa_type(0) : mantissa >> shift;

			T d_hi = support::fp::FPBits<T>::create_value(sign, static_cast<output_bits_t>(exp_hi),
														  (static_cast<output_bits_t>(m_hi) & support::fp::FPBits<T>::significand_mask) | implicit_mask)
						 .get_val();

			mantissa_type round_mask  = shift > mantissa_type::BITS ? 0 : mantissa_type(1) << (shift - 1);
			mantissa_type sticky_mask = round_mask - mantissa_type(1);

			const bool round_bit  = !(mantissa & round_mask).is_zero();
			const bool sticky_bit = !(mantissa & sticky_mask).is_zero();
			int round_and_sticky  = (static_cast<int>(round_bit) * 2) + static_cast<int>(sticky_bit);

			T d_lo{}; // GCC will lose its mind if {} is not used here.

			if (CCM_UNLIKELY(exp_lo <= 0))
			{
				// d_lo is denormal, but the output is normal.
				int scale_up_exponent = 1 - exp_lo;
				T scale_up_factor	  = support::fp::FPBits<T>::create_value(
										Sign::POS, static_cast<output_bits_t>(support::fp::FPBits<T>::exponent_bias + scale_up_exponent), implicit_mask)
										.get_val();
				T scale_down_factor = support::fp::FPBits<T>::create_value(
										  Sign::POS, static_cast<output_bits_t>(support::fp::FPBits<T>::exponent_bias - scale_up_exponent), implicit_mask)
										  .get_val();

				d_lo = support::fp::FPBits<T>::create_value(sign, static_cast<output_bits_t>(exp_lo + scale_up_exponent), implicit_mask).get_val();

				return ccm::support::multiply_add(d_lo, T(round_and_sticky), d_hi * scale_up_factor) * scale_down_factor;
			}

			d_lo = support::fp::FPBits<T>::create_value(sign, static_cast<output_bits_t>(exp_lo), implicit_mask).get_val();

			// Still correct without FMA instructions if `d_lo` is not underflow.
			T r = ccm::support::multiply_add(d_lo, T(round_and_sticky), d_hi);

			if (CCM_UNLIKELY(denorm))
			{
				// Exponent before rounding is in denormal range, simply clear the
				// exponent field.
				auto clear_exp		 = static_cast<output_bits_t>(output_bits_t(exp_hi) << support::fp::FPBits<T>::significand_length);
				output_bits_t r_bits = support::fp::FPBits<T>(r).uintval() - clear_exp;

				if (!(r_bits & support::fp::FPBits<T>::exponent_mask))
				{
					// Output is denormal after rounding, clear the implicit bit for 80-bit
					// long double.
					r_bits -= implicit_mask;

					// TODO: IEEE Std 754-2019 lets implementers choose whether to check for
					// "tininess" before or after rounding for base-2 formats, as long as
					// the same choice is made for all operations.
					// Our choice to check after rounding might not be the same as the hardware's.
					if constexpr (ShouldSignalExceptions && round_and_sticky)
					{
						support::fenv::set_errno_if_required(ERANGE);
						support::fenv::raise_except_if_required(FE_UNDERFLOW);
					}
				}

				return support::fp::FPBits<T>(r_bits).get_val();
			}

			return r;
		}

		/**
		 * @brief Converts this normalized DyadicFloat to a floating-point type \p T.
		 *
		 * This function is a thin wrapper around fast_as<T, ShouldSignalExceptions>(),
		 * ensuring that the result is correctly rounded according to the current rounding mode.
		 * It assumes that the internal representation (sign, exponent, mantissa) is already
		 * normalized.
		 *
		 * @tparam T A floating-point type (e.g., float, double, etc.) satisfying
		 *           \code std::is_floating_point_v<T> && (support::fp::FPBits<T>::fraction_length < Bits) \endcode.
		 * @tparam ShouldSignalExceptions If true, overflow or underflow conditions may
		 *                                raise floating-point exceptions (e.g., FE_OVERFLOW, FE_UNDERFLOW)
		 *                                and set \c errno if required.
		 *
		 * @return A floating-point value of type \p T that reflects the conversion.
		 */
		template <typename T, bool ShouldSignalExceptions,
				  typename = std::enable_if_t<std::is_floating_point_v<T> && (support::fp::FPBits<T>::fraction_length < Bits), void>>
		constexpr T as() const
		{
			return fast_as<T, ShouldSignalExceptions>();
		}

		/**
		 * @brief Explicitly converts this DyadicFloat to the floating-point type \p T,
		 *        without signaling floating-point exceptions.
		 *
		 * This operator invokes as<T, false>(), meaning
		 * it will not raise exceptions or set \c errno on overflow or underflow.
		 * If you need to handle exceptions, use as<T, true>() instead.
		 *
		 * @tparam T A floating-point type (e.g., float, double, etc.) satisfying
		 *           \code std::is_floating_point_v<T> && (support::fp::FPBits<T>::fraction_length < Bits) \endcode.
		 *
		 * @return The resulting value of type \p T.
		 */
		template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T> && (support::fp::FPBits<T>::fraction_length < Bits), void>>
		explicit constexpr operator T() const
		{
			return as<T, false>();
		}

		constexpr mantissa_type as_mantissa_type() const
		{
			if (mantissa.is_zero()) { return 0; }

			mantissa_type new_mant = mantissa;
			if (exponent > 0) { new_mant <<= exponent; }
			else { new_mant >>= (-exponent); }

			if (sign.is_neg()) { new_mant = (~new_mant) + 1; }

			return new_mant;
		}
	};

	/**
	 * @brief Adds two DyadicFloat values with rounding toward zero, then normalizes
	 * the result.
	 *
	 * The algorithm:
	 *   - Align the exponents so that
	 *     \code{.cpp}
	 *     newA.exponent = newB.exponent = max(a.exponent, b.exponent);
	 *     \endcode
	 *   - Add or subtract the mantissas depending on the sign.
	 *   - Normalize the result.
	 *
	 * The absolute error compared to the exact sum is bounded by:
	 * \f[
	 *   \big|\text{quick\_add}(a, b) - (a + b)\big| < \text{MSB}(a + b)\times2^{(-\text{Bits} + 2)},
	 * \f]
	 * which corresponds to up to 2 ULPs of error.
	 *
	 * @note
	 * It is assumed that both inputs are already normalized by their constructors
	 * or other functions. If not, the results can lose precision significantly.
	 *
	 * @tparam Bits The number of bits used in the DyadicFloat.
	 * @param a First DyadicFloat operand.
	 * @param b Second DyadicFloat operand.
	 * @return A DyadicFloat<Bits> representing the approximate sum of \p a and \p b.
	 */
	template <size_t Bits>
	constexpr DyadicFloat<Bits> quick_add(DyadicFloat<Bits> a, DyadicFloat<Bits> b)
	{
		if (CCM_UNLIKELY(a.mantissa.is_zero())) { return b; }
		if (CCM_UNLIKELY(b.mantissa.is_zero())) { return a; }

		// Align exponents
		if (a.exponent > b.exponent) { b.shift_right(a.exponent - b.exponent); }
		else if (b.exponent > a.exponent) { a.shift_right(b.exponent - a.exponent); }

		DyadicFloat<Bits> result;

		if (a.sign == b.sign)
		{
			// Addition
			result.sign		= a.sign;
			result.exponent = a.exponent;
			result.mantissa = a.mantissa;
			if (result.mantissa.add_overflow(b.mantissa))
			{
				// Mantissa addition overflow.
				result.shift_right(1);
				result.mantissa.val[DyadicFloat<Bits>::mantissa_type::WORD_COUNT - 1] |= (static_cast<std::uint64_t>(1) << 63);
			}
			// The result is already normalized.
			return result;
		}

		// Subtraction
		if (a.mantissa >= b.mantissa)
		{
			result.sign		= a.sign;
			result.exponent = a.exponent;
			result.mantissa = a.mantissa - b.mantissa;
		}
		else
		{
			result.sign		= b.sign;
			result.exponent = b.exponent;
			result.mantissa = b.mantissa - a.mantissa;
		}

		return result.normalize();
	}

	/**
	 * @brief
	 * Slightly less accurate but efficient multiplication of two dyadic floats with
	 * rounding toward 0, followed by normalization of the output.
	 *
	 * The resulting calculations are:
	 *   - result.exponent = a.exponent + b.exponent + Bits
	 *   - result.mantissa = quick_mul_hi(a.mantissa + b.mantissa)
	 *                      ~ (full product a.mantissa * b.mantissa) >> Bits
	 *
	 * @note
	 * The error compared to the exact mathematical product is bounded by:
	 *   2 * errors of quick_mul_hi = 2 * (UInt<Bits>::WORD_COUNT - 1) in ULPs.
	 *
	 * @warning
	 * It is assumed that both inputs are normalized by their constructors or other functions.
	 * If they are not normalized, this function's results may lose
	 * precision significantly.
	 *
	 * @tparam Bits The number of bits used in the DyadicFloat.
	 * @param a First dyadic float.
	 * @param b Second dyadic float.
	 * @return A new DyadicFloat<Bits> representing the approximate product.
	 */
	template <size_t Bits>
	constexpr DyadicFloat<Bits> quick_mul(DyadicFloat<Bits> a, DyadicFloat<Bits> b)
	{
		DyadicFloat<Bits> result;
		result.sign		= (a.sign != b.sign) ? Sign::NEG : Sign::POS;
		result.exponent = a.exponent + b.exponent + static_cast<int>(Bits);

		if (!(a.mantissa.is_zero() || b.mantissa.is_zero()))
		{
			result.mantissa = a.mantissa.quick_mul_hi(b.mantissa);
			// Check the leading bit directly, should be faster than using clz in
			// normalize().
			if (result.mantissa.val[DyadicFloat<Bits>::mantissa_type::WORD_COUNT - 1] >> 63 == 0) { result.shift_left(1); }
		}
		else { result.mantissa = static_cast<typename DyadicFloat<Bits>::mantissa_type>(0); }
		return result;
	}

	/**
	 * @brief Simple polynomial approximation using multiply-add.
	 *
	 * This function computes the product of @p a and @p b via quick_mul(),
	 * then adds @p c to the result using quick_add().
	 *
	 * @tparam Bits The number of bits used in the DyadicFloat.
	 * @param a First dyadic float.
	 * @param b Second dyadic float.
	 * @param c The dyadic float to be added to the product of \p a and \p b.
	 * @return A new DyadicFloat<Bits> containing the result of \p c + (\p a * \p b).
	 */
	template <size_t Bits>
	constexpr DyadicFloat<Bits> multiply_add(const DyadicFloat<Bits> & a, const DyadicFloat<Bits> & b, const DyadicFloat<Bits> & c)
	{
		return quick_add(c, quick_mul(a, b));
	}

	/**
	 * @brief Simple exponentiation implementation for printing or other use.
	 *
	 * This function computes \f$a^\text{power}\f$ by repeatedly squaring @p a
	 * (stored as @p cur_power) and multiplying it into @p result when the current
	 * bit of @p power is set (using quick_mul()). Only positive exponents are
	 * supported since division is not implemented.
	 *
	 * @tparam Bits The number of bits used in the DyadicFloat.
	 * @param a The base DyadicFloat.
	 * @param power The positive exponent.
	 * @return A DyadicFloat<Bits> representing \f$a^\text{power}\f$.
	 */
	template <size_t Bits>
	constexpr DyadicFloat<Bits> pow_n(DyadicFloat<Bits> a, uint32_t power)
	{
		DyadicFloat<Bits> result	= 1.0;
		DyadicFloat<Bits> cur_power = a;

		while (power > 0)
		{
			if ((power % 2) > 0) { result = quick_mul(result, cur_power); }
			power	  = power >> 1;
			cur_power = quick_mul(cur_power, cur_power);
		}
		return result;
	}

	/**
	 * @brief Multiplies a DyadicFloat by \f$2^{\text{pow\_2}}\f$.
	 *
	 * This function modifies the exponent of @p a by adding @p pow_2 to effectively
	 * multiply @p a by \f$2^{\text{pow\_2}}\f$.
	 *
	 * @tparam Bits The number of bits used in the DyadicFloat.
	 * @param a The DyadicFloat to be scaled.
	 * @param pow_2 The exponent shift. A positive value means multiplying by
	 * \f$2^{\text{pow\_2}}\f$, while a negative value means dividing by
	 * \f$2^{-\text{pow\_2}}\f$.
	 * @return The result of \f$a \times 2^{\text{pow\_2}}\f$.
	 */
	template <size_t Bits>
	constexpr DyadicFloat<Bits> mul_pow_2(DyadicFloat<Bits> a, int32_t pow_2)
	{
		DyadicFloat<Bits> result = a;
		result.exponent += pow_2;
		return result;
	}
} // namespace ccm::types
