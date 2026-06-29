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

#ifdef CCM_TYPES_LONG_DOUBLE_IS_FLOAT80

	#include "ccmath/internal/math/generic/func/power/pow_impl/powl_ld80_tables.hpp"
	#include "ccmath/internal/predef/has_builtin.hpp"
	#include "ccmath/internal/support/fenv/fenv_support.hpp"
	#include "ccmath/internal/support/fp/fp_bits.hpp"
	#include "ccmath/internal/support/fp/nearest_integer.hpp"
	#include "ccmath/internal/support/helpers/internal_ldexp.hpp"
	#include "ccmath/internal/support/is_constant_evaluated.hpp"
	#include "ccmath/internal/support/multiply_add.hpp"
	#include "ccmath/internal/support/poly_eval.hpp"
	#include "ccmath/internal/types/number_pair.hpp"

	#include <cfloat>
	#include <cstdint>
	#include <limits>

namespace ccm::gen::internal::impl::bit80
{
	namespace powl_ld80_detail
	{
		using FPBits_t		 = support::fp::FPBits<long double>;
		using LongDoublePair = types::NumberPair<long double>;
		namespace tables	 = ccm::gen::impl::internal::impl::powl_ld80_tables;

		inline constexpr long double kScaleUp			 = 0x1.0p512L;
		inline constexpr long double kScaleDown			 = 0x1.0p-512L;
		inline constexpr long double kUpperY6Bound		 = 512.0L * 64.0L;
		inline constexpr long double kOverflowY6HiClamp	 = (static_cast<long double>(LDBL_MAX_EXP - 2) + 513.0L) * 64.0L;
		inline constexpr long double kUnderflowY6HiClamp = (static_cast<long double>(LDBL_MIN_EXP - (LDBL_MANT_DIG - 1)) + 512.0L) * 64.0L;

		constexpr LongDoublePair exact_add(long double a, long double b) noexcept
		{
			LongDoublePair r{ 0.0L, 0.0L };
			r.hi				= a + b;
			const long double t = r.hi - a;
			r.lo				= b - t;
			return r;
		}

		constexpr LongDoublePair split(long double a) noexcept
		{
			// Velkamp splitting for the 64-bit x87 significand.
			constexpr long double split_c = 0x1.0p33L + 1.0L;
			const long double t1		  = split_c * a;
			const long double t2		  = a - t1;
			return LongDoublePair{ t1 + t2, a - (t1 + t2) };
		}

		constexpr LongDoublePair exact_mult(long double a, long double b) noexcept
		{
			LongDoublePair r{ 0.0L, 0.0L };
			const LongDoublePair as = split(a);
			const LongDoublePair bs = split(b);
			r.hi					= a * b;
			const long double t1	= as.hi * bs.hi - r.hi;
			const long double t2	= as.hi * bs.lo + t1;
			const long double t3	= as.lo * bs.hi + t2;
			r.lo					= as.lo * bs.lo + t3;
			return r;
		}

		constexpr long double nearest_integer(long double x) noexcept
		{
			if (!support::is_constant_evaluated())
			{
	#if CCM_HAS_BUILTIN(__builtin_roundl)
				return __builtin_roundl(x);
	#else
				return static_cast<long double>(support::fp::nearest_integer(static_cast<double>(x)));
	#endif
			}

			if (constexpr long double max_exact_int = 0x1.0p63L; x < max_exact_int && x > -max_exact_int)
			{
				constexpr long double round_offset = 0x1.0p62L;
				return x < 0.0L ? (x - round_offset) + round_offset : (x + round_offset) - round_offset;
			}
			return x;
		}

		inline constexpr long double kScaleStep = 512.0L * 64.0L;

		constexpr long double powl_ld80_general_finite(long double base, long double exp) noexcept
		{
			FPBits_t base_bits(base);
			int exponent_adjust = 0;

			if (base_bits.is_subnormal())
			{
				base			= support::helpers::internal_ldexp(base, LDBL_MANT_DIG - 1);
				base_bits		= FPBits_t(base);
				exponent_adjust = -(LDBL_MANT_DIG - 1);
			}

			const int e_x = static_cast<int>(base_bits.get_biased_exponent()) - static_cast<int>(FPBits_t::exponent_bias) + exponent_adjust;

			typename FPBits_t::storage_type sig = base_bits.get_mantissa();
			if (base_bits.get_implicit_bit())
			{
				sig |= FPBits_t::EXPLICIT_BIT_MASK;
			}

			const unsigned idx_x = static_cast<unsigned>((sig & FPBits_t::fraction_mask) >> (FPBits_t::fraction_length - 7));

			const typename FPBits_t::storage_type m_sig = sig & FPBits_t::significand_mask;
			const long double m_x = FPBits_t::create_value(types::Sign::POS, FPBits_t::exponent_bias, m_sig | FPBits_t::EXPLICIT_BIT_MASK).get_val();

			long double dx = 0.0L;
			LongDoublePair dx_c0{ 0.0L, 0.0L };

			const typename FPBits_t::storage_type frac_mask_high =
				FPBits_t::fraction_mask & ~((static_cast<typename FPBits_t::storage_type>(1) << (FPBits_t::fraction_length - 7)) - 1);
			const typename FPBits_t::storage_type c_sig = (FPBits_t(m_x).get_mantissa() & frac_mask_high) | FPBits_t::EXPLICIT_BIT_MASK;
			const long double c							= FPBits_t::create_value(types::Sign::POS, FPBits_t::exponent_bias, c_sig).get_val();
			dx	  = support::multiply_add(tables::RD.at(static_cast<std::size_t>(idx_x)), m_x - c, tables::CD.at(static_cast<std::size_t>(idx_x)));
			dx_c0 = exact_mult(tables::POW_LOG2_COEFFS[0], dx);

			const long double dx2 = dx * dx;
			const long double c0  = support::multiply_add(dx, tables::POW_LOG2_COEFFS[2], tables::POW_LOG2_COEFFS[1]);
			const long double c1  = support::multiply_add(dx, tables::POW_LOG2_COEFFS[4], tables::POW_LOG2_COEFFS[3]);
			const long double c2  = support::multiply_add(dx, tables::POW_LOG2_COEFFS[6], tables::POW_LOG2_COEFFS[5]);
			const long double p	  = support::polyeval(dx2, c0, c1, c2, tables::POW_LOG2_COEFFS[7]);

			const long double log2_r_hi	 = tables::LOG2_R_HI.at(static_cast<std::size_t>(idx_x));
			const long double log2_r_mid = tables::LOG2_R_MID.at(static_cast<std::size_t>(idx_x));
			const long double log2_r_lo	 = tables::LOG2_R_LO.at(static_cast<std::size_t>(idx_x));
			LongDoublePair log2_x_hi	 = exact_add(static_cast<long double>(e_x) + log2_r_hi, dx_c0.hi);
			const long double log2_x_lo	 = support::multiply_add(dx2, p, dx_c0.lo + log2_r_mid);

			LongDoublePair log2_x = exact_add(log2_x_hi.hi, log2_x_lo);
			log2_x.lo += log2_x_hi.lo + log2_r_lo;

			const LongDoublePair exp_split = split(exp);
			const long double y6_hi		   = exp_split.hi * 0x1.0p6L;
			const long double y6_lo		   = exp_split.lo * 0x1.0p6L;

			LongDoublePair y6_log2_hi = exact_mult(y6_hi, log2_x.hi);
			y6_log2_hi.lo			  = support::multiply_add(y6_hi, log2_x.lo, y6_log2_hi.lo);
			LongDoublePair y6_log2_lo = exact_mult(y6_lo, log2_x.hi);
			y6_log2_lo.lo			  = support::multiply_add(y6_lo, log2_x.lo, y6_log2_lo.lo);

			LongDoublePair y6_log2_x = exact_add(y6_log2_hi.hi, y6_log2_lo.hi);
			y6_log2_x.lo += y6_log2_hi.lo + y6_log2_lo.lo;

			int scale_exp512	  = 0;
			bool forced_overflow  = false;
			bool forced_underflow = false;
			bool scaled_down	  = false;

			while (FPBits_t(y6_log2_x.hi).abs().get_val() >= kUpperY6Bound)
			{
				if (FPBits_t(y6_log2_x.hi).sign().is_pos())
				{
					scale_exp512 += 512;
					y6_log2_x.hi -= kScaleStep;
					if (y6_log2_x.hi > kOverflowY6HiClamp)
					{
						y6_log2_x.hi	= kOverflowY6HiClamp;
						y6_log2_x.lo	= 0.0L;
						forced_overflow = true;
						break;
					}
				} else
				{
					scale_exp512 -= 512;
					scaled_down = true;
					y6_log2_x.hi += kScaleStep;
					if (y6_log2_x.hi < kUnderflowY6HiClamp)
					{
						y6_log2_x.hi	 = kUnderflowY6HiClamp;
						y6_log2_x.lo	 = 0.0L;
						forced_underflow = true;
						break;
					}
				}
			}

			// Saturate with a genuine over/underflowing product so the active rounding mode
			// decides the result: round to nearest and rounding toward the result sign give
			// inf/zero, the opposing directed modes give max_normal/min_subnormal. A hard
			// inf/zero here would be wrong under directed rounding (the binary64 kernel lets
			// result * scale round naturally for the same reason). The magnitude is far past
			// the representable range, so the exact factors do not matter.
			// The result here is positive (the caller applies the sign) and far outside the
			// representable range. Saturate by scaling a positive unit past the range so the
			// hardware conversion rounds in the active mode: round to nearest and rounding toward
			// the result sign give inf/zero, the opposing directed modes give max_normal and
			// min_subnormal. A hard inf/zero would be wrong under directed rounding, mirroring how
			// the binary64 kernel lets result * scale round naturally. The shifts stay inside the
			// internal_ldexp fast range so the rounding comes from the conversion, not a rounding
			// mode read (which a fenv-unaware optimizer may hoist). Constant evaluation always
			// rounds to nearest, so the compile-time path keeps the inf/zero saturation.
			constexpr int kOverflowShift  = LDBL_MAX_EXP;					  // 2^LDBL_MAX_EXP overflows.
			constexpr int kUnderflowShift = LDBL_MIN_EXP - LDBL_MANT_DIG - 1; // half the smallest subnormal.
			if (forced_overflow)
			{
				support::fenv::set_errno_if_required(ERANGE);
				support::fenv::raise_except_if_required(FE_OVERFLOW);
				if (!support::is_constant_evaluated())
				{
					return support::helpers::internal_ldexp(1.0L, kOverflowShift);
				}
				return FPBits_t::inf(types::Sign::POS).get_val();
			}

			if (forced_underflow)
			{
				support::fenv::set_errno_if_required(ERANGE);
				support::fenv::raise_except_if_required(FE_UNDERFLOW);
				if (!support::is_constant_evaluated())
				{
					return support::helpers::internal_ldexp(1.0L, kUnderflowShift);
				}
				return FPBits_t::zero(types::Sign::POS).get_val();
			}

			// Extract the integer part from the high limb only: hm is near y6_log2_x.hi, so
			// (hi - hm) is exact (Sterbenz) and the low limb is added separately. Collapsing
			// hi + lo into a single long double first would round at the magnitude of hm and
			// drop ~log2(hm) low bits of lo6, which dominates the error for large exponents.
			const long double hm  = nearest_integer(y6_log2_x.hi);
			const long double lo6 = (y6_log2_x.hi - hm) + y6_log2_x.lo;

			const int hm_i = static_cast<int>(hm);

			const unsigned idx_y								= static_cast<unsigned>(hm_i) & 0x3fU;
			const typename FPBits_t::storage_type exp2_hi_i		= static_cast<typename FPBits_t::storage_type>(static_cast<std::int64_t>(hm_i >> 6))
																  << FPBits_t::significand_length;
			const typename FPBits_t::storage_type exp2_mid_hi_i = FPBits_t(tables::EXP2_HI.at(static_cast<std::size_t>(idx_y))).uintval();
			const typename FPBits_t::storage_type exp2_mid_lo_i =
				idx_y != 0U ? FPBits_t(tables::EXP2_MID.at(static_cast<std::size_t>(idx_y))).uintval() : typename FPBits_t::storage_type{};

			const typename FPBits_t::storage_type exp2_hm_hi_i = exp2_hi_i + exp2_mid_hi_i;
			const typename FPBits_t::storage_type exp2_hm_lo_i = idx_y != 0U ? exp2_hi_i + exp2_mid_lo_i : typename FPBits_t::storage_type{};

			const long double exp2_hm_hi = FPBits_t(exp2_hm_hi_i).get_val();
			const long double exp2_hm_lo = idx_y != 0U ? FPBits_t(exp2_hm_lo_i).get_val() : 0.0L;

			const long double lo6_sqr = lo6 * lo6;
			const long double d0	  = support::multiply_add(lo6, tables::POW_EXP2_COEFFS[2], tables::POW_EXP2_COEFFS[1]);
			const long double d1	  = support::multiply_add(lo6, tables::POW_EXP2_COEFFS[4], tables::POW_EXP2_COEFFS[3]);
			const long double d2	  = support::multiply_add(lo6, tables::POW_EXP2_COEFFS[6], tables::POW_EXP2_COEFFS[5]);
			const long double pp	  = support::polyeval(lo6_sqr, d0, d1, d2);

			long double result = support::multiply_add(exp2_hm_hi * lo6, pp, exp2_hm_lo);
			// exp2_hm_hi + exp2_hm_lo represents 2^(hm/64) to roughly twice the significand width, so
			// the low limb must also be scaled by (2^(lo6/64) - 1) = lo6 * pp. Omitting this term (as
			// the binary64 pow kernel does) leaks ~2^-60 of relative error, which is below a double ULP
			// but reaches several long double ULP for large |lo6| with a non-zero exp2 mid entry.
			result = support::multiply_add(exp2_hm_lo * lo6, pp, result);
			result += exp2_hm_hi;

			long double final = result;
			if (scale_exp512 != 0)
			{
				final = support::helpers::internal_ldexp(final, scale_exp512);
			}
			FPBits_t final_bits(final);
			if (final_bits.sign().is_neg())
			{
				if (final_bits.is_inf())
				{
					support::fenv::set_errno_if_required(ERANGE);
					support::fenv::raise_except_if_required(FE_OVERFLOW);
					final = FPBits_t::inf(types::Sign::POS).get_val();
				} else if (!final_bits.is_zero() && final_bits.abs().uintval() < FPBits_t::min_subnormal().uintval())
				{
					support::fenv::set_errno_if_required(ERANGE);
					support::fenv::raise_except_if_required(FE_UNDERFLOW);
					final = 0.0L;
				} else
				{
					final = final_bits.abs().get_val();
				}
			} else if (scale_exp512 > 0 && final_bits.is_inf() && final_bits.is_pos())
			{
				support::fenv::set_errno_if_required(ERANGE);
				support::fenv::raise_except_if_required(FE_OVERFLOW);
			} else if (scaled_down && final_bits.is_finite() && !final_bits.is_zero() && final_bits.abs().uintval() < FPBits_t::min_subnormal().uintval())
			{
				support::fenv::set_errno_if_required(ERANGE);
				support::fenv::raise_except_if_required(FE_UNDERFLOW);
				final = 0.0L;
			}

			return final;
		}

	} // namespace powl_ld80_detail

	constexpr long double powl_ld80_general_finite(long double base, long double exp) noexcept
	{
		return powl_ld80_detail::powl_ld80_general_finite(base, exp);
	}

} // namespace ccm::gen::internal::impl::bit80

#endif
