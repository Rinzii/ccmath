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

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fenv/rounding_mode.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/types/big_int.hpp"
#include "ccmath/internal/types/dyadic_float.hpp"
#include "ccmath/internal/types/sign.hpp"

#include <cfenv>
#include <cstddef>
#include <type_traits>

namespace ccm::support::fp
{
	namespace fma_internal
	{
		// Extract the sticky bits and shift `mant` to the right by `shift_length`.
		// Returns true if any non-zero bit was shifted out.
		template <typename T>
		[[nodiscard]] constexpr bool shift_mantissa(int shift_length, T & mant)
		{
			if (shift_length >= static_cast<int>(T::BITS))
			{
				mant = T(0);
				return true; // mant was non-zero before the shift.
			}
			const T mask		   = (T(1) << static_cast<std::size_t>(shift_length)) - T(1);
			const bool sticky_bits = (mant & mask) != T(0);
			mant >>= static_cast<std::size_t>(shift_length);
			return sticky_bits;
		}
	} // namespace fma_internal

	// Correctly-rounded software fused multiply-add for every rounding mode. Ported from LLVM
	// libc's generic FMA (libc/src/__support/FPUtil/generic/FMA.h). The product of the two explicit
	// mantissas is formed exactly in a wide integer, the addend is aligned with a sticky bit, the
	// sum is computed once, and the wide result is rounded a single time through DyadicFloat::as,
	// which honors the active rounding mode. This is used where the target has neither a fused
	// hardware FMA nor __builtin_fma (e.g. MSVC on a generic x86-64 build), keeping the error-free
	// transforms exact without falling back to a non-fused (x * y) + z. Unlike LLVM's float
	// specialization this path is correctly rounded in directed modes for float as well, so all
	// floating types share it.
	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<std::is_floating_point_v<T>, T> generic_fma(T x, T y, T z)
	{
		using FPBits	  = support::fp::FPBits<T>;
		using StorageType = typename FPBits::storage_type;
		using Sign		  = types::Sign;

		constexpr int IN_EXPLICIT_MANT_LEN	 = static_cast<int>(FPBits::fraction_length) + 1;
		constexpr std::size_t PROD_LEN		 = 2U * static_cast<std::size_t>(IN_EXPLICIT_MANT_LEN);
		constexpr std::size_t TMP_RESULT_LEN = support::bit_ceil(PROD_LEN + 1U);
		using TmpResultType					 = types::UInt<TMP_RESULT_LEN>;
		using WideFloat						 = types::DyadicFloat<TMP_RESULT_LEN>;

		FPBits x_bits(x);
		FPBits y_bits(y);
		FPBits z_bits(z);

		if (CCM_UNLIKELY(x_bits.is_nan() || y_bits.is_nan() || z_bits.is_nan()))
		{
			if (x_bits.is_signaling_nan() || y_bits.is_signaling_nan() || z_bits.is_signaling_nan()) { support::fenv::raise_except_if_required(FE_INVALID); }
			if (x_bits.is_quiet_nan()) { return FPBits::quiet_nan(x_bits.sign(), x_bits.get_mantissa()).get_val(); }
			if (y_bits.is_quiet_nan()) { return FPBits::quiet_nan(y_bits.sign(), y_bits.get_mantissa()).get_val(); }
			if (z_bits.is_quiet_nan()) { return FPBits::quiet_nan(z_bits.sign(), z_bits.get_mantissa()).get_val(); }
			return FPBits::quiet_nan().get_val();
		}

		if (CCM_UNLIKELY(x == T(0) || y == T(0) || z == T(0))) { return x * y + z; }

		int x_exp = 0;
		int y_exp = 0;
		int z_exp = 0;

		// Denormal scaling = 2^(fraction length): create_value with a zero stored fraction (and the
		// explicit integer bit for formats that store it) gives exactly that power of two.
		const StorageType implicit_mask = static_cast<StorageType>(FPBits::significand_mask - FPBits::fraction_mask);
		const T denormal_scaling =
			FPBits::create_value(Sign::POS, static_cast<StorageType>(static_cast<int>(FPBits::fraction_length) + FPBits::exponent_bias), implicit_mask)
				.get_val();

		if (CCM_UNLIKELY(x_bits.is_subnormal()))
		{
			x_exp -= static_cast<int>(FPBits::fraction_length);
			x *= denormal_scaling;
		}
		if (CCM_UNLIKELY(y_bits.is_subnormal()))
		{
			y_exp -= static_cast<int>(FPBits::fraction_length);
			y *= denormal_scaling;
		}
		if (CCM_UNLIKELY(z_bits.is_subnormal()))
		{
			z_exp -= static_cast<int>(FPBits::fraction_length);
			z *= denormal_scaling;
		}

		x_bits			  = FPBits(x);
		y_bits			  = FPBits(y);
		z_bits			  = FPBits(z);
		const Sign z_sign = z_bits.sign();
		Sign prod_sign	  = (x_bits.sign() == y_bits.sign()) ? Sign::POS : Sign::NEG;
		x_exp += static_cast<int>(x_bits.get_biased_exponent());
		y_exp += static_cast<int>(y_bits.get_biased_exponent());
		z_exp += static_cast<int>(z_bits.get_biased_exponent());

		if (CCM_UNLIKELY(x_exp == FPBits::max_biased_exponent || y_exp == FPBits::max_biased_exponent || z_exp == FPBits::max_biased_exponent))
		{
			if (CCM_UNLIKELY(x_exp != FPBits::max_biased_exponent && y_exp != FPBits::max_biased_exponent && z_bits.is_inf())) { return z; }
			return x * y + z;
		}

		// Extract mantissas and append the hidden leading bit.
		const StorageType x_mant = x_bits.get_explicit_mantissa();
		const StorageType y_mant = y_bits.get_explicit_mantissa();
		TmpResultType z_mant	 = TmpResultType(z_bits.get_explicit_mantissa());

		TmpResultType prod_mant = TmpResultType(x_mant) * TmpResultType(y_mant);
		int prod_lsb_exp		= x_exp + y_exp - (FPBits::exponent_bias + 2 * static_cast<int>(FPBits::fraction_length));

		// Left-shift the addend so that, after aligning the least significant bits, the worst-case
		// cancellation still has room to store the exact result before rounding.
		constexpr int RESULT_MIN_LEN = static_cast<int>(PROD_LEN) - static_cast<int>(FPBits::fraction_length);
		z_mant <<= static_cast<std::size_t>(RESULT_MIN_LEN);
		int z_lsb_exp	 = z_exp - (static_cast<int>(FPBits::fraction_length) + RESULT_MIN_LEN);
		bool sticky_bits = false;
		bool z_shifted	 = false;

		// Align exponents to the lower of the two least-significant-bit exponents.
		if (prod_lsb_exp < z_lsb_exp)
		{
			sticky_bits	 = fma_internal::shift_mantissa(z_lsb_exp - prod_lsb_exp, prod_mant);
			prod_lsb_exp = z_lsb_exp;
		}
		else if (z_lsb_exp < prod_lsb_exp)
		{
			z_shifted	= true;
			sticky_bits = fma_internal::shift_mantissa(prod_lsb_exp - z_lsb_exp, z_mant);
		}

		// Perform (-1)^prod_sign * prod_mant + (-1)^z_sign * z_mant into (prod_sign, prod_mant).
		if (prod_sign == z_sign) { prod_mant += z_mant; }
		else
		{
			if (prod_mant >= z_mant)
			{
				// Keep the sticky bits of the subtrahend positive to simplify rounding.
				if (z_shifted && sticky_bits) { ++z_mant; }
				prod_mant -= z_mant;
			}
			else
			{
				if (!z_shifted && sticky_bits) { ++prod_mant; }
				prod_mant = z_mant - prod_mant;
				prod_sign = z_sign;
			}
		}

		if (prod_mant == TmpResultType(0))
		{
			// Exact cancellation (x*y == -z): -0.0 when rounding downward, +0.0 otherwise.
			prod_sign = (support::fenv::get_rounding_mode() == FE_DOWNWARD) ? Sign::NEG : Sign::POS;
		}

		WideFloat result(prod_sign, prod_lsb_exp - FPBits::exponent_bias, prod_mant);
		if (sticky_bits) { result.mantissa |= TmpResultType(1); }
		// generic_as performs the single rounding with explicit round/sticky integer logic keyed on
		// support::fenv::get_rounding_mode(). Unlike fast_as it does not lean on the hardware to round,
		// so it stays correct during constant evaluation, where it follows CCM_CONSTEXPR_ROUNDING_MODE.
		// ShouldSignalExceptions is false so an error-free transform raises no spurious FE_INEXACT.
		return result.template generic_as<T, /*ShouldSignalExceptions=*/false>();
	}
} // namespace ccm::support::fp
