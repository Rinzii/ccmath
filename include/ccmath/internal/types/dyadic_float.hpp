/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

// Code borrowed from LLVM with heavy modifications done for ccmath to allow for both cross-platform and cross-compiler support.
// https://github.com/llvm/llvm-project/

#pragma once

#include "ccmath/internal/support/fp_bits.hpp"
#include "ccmath/internal/support/multiply_add.hpp"
#include "ccmath/internal/support/type_traits.hpp"
#include "ccmath/internal/types/big_int.hpp"

#include <cassert>
#include <cstddef>

namespace ccm::types
{

	/**
	 * @brief A class for high precision floating point computations using dyadic format.
	 *
	 * This class represents high precision floating point values in dyadic format, consisting of three fields:
	 * - sign: A boolean value where false indicates positive and true indicates negative.
	 * - exponent: The exponent value of the least significant bit of the mantissa.
	 * - mantissa: An unsigned integer of length `Bits`.
	 *
	 * The stored value is calculated as:
	 *   real value = (-1)^sign * 2^exponent * (mantissa as unsigned integer)
	 *
	 * The data is considered normalized if, for a non-zero mantissa, the leading bit is 1.
	 * Constructors and most functions ensure that the outputs are normalized. To simplify and improve efficiency,
	 * many functions assume that the inputs are already normalized.
	 *
	 * @tparam Bits The length of the mantissa in bits.
	 */
	template <size_t Bits>
	struct DyadicFloat
	{
		using mantissa_type = UInt<Bits>;

		Sign sign				= Sign::POS;
		int exponent			= 0;
		mantissa_type mantissa	= mantissa_type(0);

		constexpr DyadicFloat() = default;

		template <typename T, std::enable_if_t<support::traits::ccm_is_floating_point_v<T>, bool> = true>
		constexpr explicit DyadicFloat(T x)
		{
			static_assert(support::FPBits<T>::fraction_length < Bits);
			support::FPBits<T> x_bits(x);

			assert(x == x_bits.get_val());

			sign	 = x_bits.sign();
			exponent = x_bits.get_explicit_exponent() - support::FPBits<T>::fraction_length;
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
				int shift_length = support::countl_zero(mantissa);
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
		constexpr DyadicFloat & shift_left(int shift_length)
		{
			exponent -= shift_length;
			mantissa <<= static_cast<std::size_t>(shift_length);
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
		constexpr DyadicFloat & shift_right(int shift_length)
		{
			exponent += shift_length;
			mantissa >>= static_cast<std::size_t>(shift_length);
			return *this;
		}

		/**
		 * @brief Returns the unbiased exponent, assuming normalization.
		 *
		 * @return The unbiased exponent.
		 */
		[[nodiscard]] constexpr int get_unbiased_exponent() const { return exponent + (Bits - 1); }

		/**
		 * @brief Converts the DyadicFloat to a floating-point type T, assuming normalization and rounding the result.
		 *
		 * This function converts the DyadicFloat to the specified floating-point type T. The conversion assumes that the
		 * value is already normalized and outputs a normalized result, correctly rounded according to the current rounding mode.
		 *
		 * @tparam T The target floating-point type.
		 * @return The floating-point representation of the DyadicFloat.
		 */
		template <typename T, typename = std::enable_if_t<ccm::support::traits::ccm_is_floating_point_v<T> && (support::FPBits<T>::fraction_length < Bits), void>>
		explicit constexpr operator T() const
		{
			if (CCM_UNLIKELY(mantissa.is_zero())) { return support::FPBits<T>::zero(sign).get_val(); }

			// Assume normalized input and output.
			constexpr uint32_t desired_precision		  = support::FPBits<T>::fraction_length + 1;
			using output_bits_t					  = typename support::FPBits<T>::storage_type;
			constexpr output_bits_t implicit_mask = support::FPBits<T>::significand_mask - support::FPBits<T>::fraction_mask;

			int exp_hi = exponent + static_cast<int>((Bits - 1) + support::FPBits<T>::exponent_bias);

			if (CCM_UNLIKELY(exp_hi > 2 * support::FPBits<T>::exponent_bias))
			{
				// Results overflow.
				T d_hi = support::FPBits<T>::create_value(sign, 2 * support::FPBits<T>::exponent_bias, implicit_mask).get_val();
				return T(2) * d_hi;
			}

			bool denorm	   = false;
			uint32_t shift = Bits - desired_precision;
			if (CCM_UNLIKELY(exp_hi <= 0))
			{
				// Output is denormal.
				denorm = true;
				shift  = (Bits - desired_precision) + static_cast<uint32_t>(1 - exp_hi);

				exp_hi = support::FPBits<T>::exponent_bias;
			}

			int exp_lo = exp_hi - static_cast<int>(desired_precision) - 1;

			std::size_t const mantissa_len = mantissa_type::BITS;

			mantissa_type m_hi = shift >= mantissa_len ? mantissa_type(0) : mantissa >> shift;

			T d_hi =
				support::FPBits<T>::create_value(sign, static_cast<output_bits_t>(exp_hi), (static_cast<output_bits_t>(m_hi) & support::FPBits<T>::significand_mask) | implicit_mask).get_val();

			mantissa_type round_mask	 = shift > mantissa_len ? 0 : mantissa_type(1) << (shift - 1);
			mantissa_type sticky_mask = round_mask - mantissa_type(1);

			bool round_bit		 = !(mantissa & round_mask).is_zero();
			bool sticky_bit		 = !(mantissa & sticky_mask).is_zero();
			int round_and_sticky = static_cast<int>(round_bit) * 2 + static_cast<int>(sticky_bit);

			T d_lo;

			if (CCM_UNLIKELY(exp_lo <= 0))
			{
				// d_lo is denormal, but the output is normal.
				int scale_up_exponent = 2 * desired_precision;
				T scale_up_factor	  = support::FPBits<T>::create_value(sign, support::FPBits<T>::exponent_bias + static_cast<output_bits_t>(scale_up_exponent), implicit_mask).get_val();
				T scale_down_factor	  = support::FPBits<T>::create_value(sign, support::FPBits<T>::exponent_bias - static_cast<output_bits_t>(scale_up_exponent), implicit_mask).get_val();

				d_lo = support::FPBits<T>::create_value(sign, static_cast<output_bits_t>(exp_lo + scale_up_exponent), implicit_mask).get_val();

				return support::multiply_add(d_lo, T(round_and_sticky), d_hi * scale_up_factor) * scale_down_factor;
			}

			d_lo = support::FPBits<T>::create_value(sign, static_cast<output_bits_t>(exp_lo), implicit_mask).get_val();

			// Still correct without FMA instructions if `d_lo` is not underflow.
			T r = support::multiply_add(d_lo, T(round_and_sticky), d_hi);

			if (CCM_UNLIKELY(denorm))
			{
				// Exponent before rounding is in denormal range, clear the exponent field.
				output_bits_t clear_exp = (output_bits_t(exp_hi) << support::FPBits<T>::significand_length);
				output_bits_t r_bits	= support::FPBits<T>(r).uintval() - clear_exp;
				if (!(r_bits & support::FPBits<T>::exponent_mask))
				{
					// Output is denormal after rounding, clear the implicit bit for 80-bit long double.
					r_bits -= implicit_mask;
				}

				return support::FPBits<T>(r_bits).get_val();
			}

			return r;
		}

		explicit constexpr operator mantissa_type() const
		{
			if (mantissa.is_zero()) { return 0; }

			mantissa_type new_mant = mantissa;
			if (exponent > 0) { new_mant <<= exponent; }
			else { new_mant >>= (-exponent); }

			if (sign.is_neg()) { new_mant = (~new_mant) + 1; }

			return new_mant;
		}
	};

	// Quick add - Add 2 dyadic floats with rounding toward 0 and then normalize the
	// output:
	//   - Align the exponents so that:
	//     new a.exponent = new b.exponent = max(a.exponent, b.exponent)
	//   - Add or subtract the mantissas depending on the signs.
	//   - Normalize the result.
	// The absolute errors compared to the mathematical sum is bounded by:
	//   | quick_add(a, b) - (a + b) | < MSB(a + b) * 2^(-Bits + 2),
	// i.e., errors are up to 2 ULPs.
	// Assume inputs are normalized (by constructors or other functions) so that we
	// don't need to normalize the inputs again in this function.  If the inputs are
	// not normalized, the results might lose precision significantly.
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
				result.mantissa.val[DyadicFloat<Bits>::mantissa_type::WORD_COUNT - 1] |= (static_cast<uint64_t>(1) << 63);
			}
			// Result is already normalized.
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

	// Quick Mul - Slightly less accurate but efficient multiplication of 2 dyadic
	// floats with rounding toward 0 and then normalize the output:
	//   result.exponent = a.exponent + b.exponent + Bits,
	//   result.mantissa = quick_mul_hi(a.mantissa + b.mantissa)
	//                   ~ (full product a.mantissa * b.mantissa) >> Bits.
	// The errors compared to the mathematical product is bounded by:
	//   2 * errors of quick_mul_hi = 2 * (UInt<Bits>::WORD_COUNT - 1) in ULPs.
	// Assume inputs are normalized (by constructors or other functions) so that we
	// don't need to normalize the inputs again in this function.  If the inputs are
	// not normalized, the results might lose precision significantly.
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

	// Simple polynomial approximation.
	template <size_t Bits>
	constexpr DyadicFloat<Bits> multiply_add(const DyadicFloat<Bits> & a, const DyadicFloat<Bits> & b, const DyadicFloat<Bits> & c)
	{
		return quick_add(c, quick_mul(a, b));
	}

	// Simple exponentiation implementation for printf. Only handles positive
	// exponents, since division isn't implemented.
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

	template <size_t Bits>
	constexpr DyadicFloat<Bits> mul_pow_2(DyadicFloat<Bits> a, int32_t pow_2)
	{
		DyadicFloat<Bits> result = a;
		result.exponent += pow_2;
		return result;
	}

} // namespace ccm::types
