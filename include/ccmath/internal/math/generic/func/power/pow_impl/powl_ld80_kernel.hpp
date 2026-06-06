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

#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)

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

	#if defined(CCMATH_TARGET_CPU_HAS_FMA)
		#define CCMATH_POW_KERNEL_USE_FMA_DX 1
	#endif

namespace ccm::gen::internal::impl::bit80
{
	namespace powl_ld80_detail
	{
		inline long double fma_dx(long double x, long double y, long double z) noexcept
		{
	#if defined(CCMATH_TARGET_CPU_HAS_FMA)
			return support::multiply_add(x, y, z);
	#elif CCM_HAS_BUILTIN(__builtin_fmal)
			return __builtin_fmal(x, y, z);
	#elif CCM_HAS_BUILTIN(__builtin_fma)
			return static_cast<long double>(__builtin_fma(static_cast<double>(x), static_cast<double>(y), static_cast<double>(z)));
	#else
			return support::multiply_add(x, y, z);
	#endif
		}

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
			r.hi = a * b;
			const long double t1	= as.hi * bs.hi - r.hi;
			const long double t2	= as.hi * bs.lo + t1;
			const long double t3	= as.lo * bs.hi + t2;
			r.lo					= as.lo * bs.lo + t3;
			return r;
		}

		constexpr long double nearest_integer(long double x) noexcept
		{
			// hm stays within exact-integer double range after y6 clamping.
			return static_cast<long double>(support::fp::nearest_integer(static_cast<double>(x)));
		}

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
			if (base_bits.get_implicit_bit()) { sig |= FPBits_t::EXPLICIT_BIT_MASK; }

			const unsigned idx_x = static_cast<unsigned>((sig & FPBits_t::fraction_mask) >> (FPBits_t::fraction_length - 7));

			const typename FPBits_t::storage_type m_sig = sig & FPBits_t::significand_mask;
			const long double m_x = FPBits_t::create_value(types::Sign::POS, FPBits_t::exponent_bias, m_sig | FPBits_t::EXPLICIT_BIT_MASK).get_val();

			long double dx = 0.0L;
			LongDoublePair dx_c0{ 0.0L, 0.0L };

			if (support::is_constant_evaluated())
			{
				const typename FPBits_t::storage_type frac_mask_high =
					FPBits_t::fraction_mask & ~((static_cast<typename FPBits_t::storage_type>(1) << (FPBits_t::fraction_length - 7)) - 1);
				const typename FPBits_t::storage_type c_sig = (FPBits_t(m_x).get_mantissa() & frac_mask_high) | FPBits_t::EXPLICIT_BIT_MASK;
				const long double c							= FPBits_t::create_value(types::Sign::POS, FPBits_t::exponent_bias, c_sig).get_val();
				dx	  = support::multiply_add(tables::RD.at(static_cast<std::size_t>(idx_x)), m_x - c, tables::CD.at(static_cast<std::size_t>(idx_x)));
				dx_c0 = exact_mult(tables::POW_LOG2_COEFFS[0], dx);
			}
			else
			{
	#ifdef CCMATH_POW_KERNEL_USE_FMA_DX
				dx	  = fma_dx(tables::RD.at(static_cast<std::size_t>(idx_x)), m_x, -1.0L);
				dx_c0 = exact_mult(tables::POW_LOG2_COEFFS[0], dx);
	#else
				const typename FPBits_t::storage_type frac_mask_high =
					FPBits_t::fraction_mask & ~((static_cast<typename FPBits_t::storage_type>(1) << (FPBits_t::fraction_length - 7)) - 1);
				const typename FPBits_t::storage_type c_sig = (FPBits_t(m_x).get_mantissa() & frac_mask_high) | FPBits_t::EXPLICIT_BIT_MASK;
				const long double c							= FPBits_t::create_value(types::Sign::POS, FPBits_t::exponent_bias, c_sig).get_val();
				dx	  = support::multiply_add(tables::RD.at(static_cast<std::size_t>(idx_x)), m_x - c, tables::CD.at(static_cast<std::size_t>(idx_x)));
				dx_c0 = exact_mult(tables::POW_LOG2_COEFFS[0], dx);
	#endif
			}

			const long double dx2 = dx * dx;
			const long double c0  = support::multiply_add(dx, tables::POW_LOG2_COEFFS[2], tables::POW_LOG2_COEFFS[1]);
			const long double c1  = support::multiply_add(dx, tables::POW_LOG2_COEFFS[4], tables::POW_LOG2_COEFFS[3]);
			const long double c2  = support::multiply_add(dx, tables::POW_LOG2_COEFFS[6], tables::POW_LOG2_COEFFS[5]);
			const long double p	  = support::polyeval(dx2, c0, c1, c2);

			const auto & log2_r			= tables::LOG2_R_TD.at(static_cast<std::size_t>(idx_x));
			LongDoublePair log2_x_hi	= exact_add(static_cast<long double>(e_x) + log2_r.hi, dx_c0.hi);
			const long double log2_x_lo = support::multiply_add(dx2, p, dx_c0.lo + static_cast<long double>(log2_r.mid));

			LongDoublePair log2_x = exact_add(log2_x_hi.hi, log2_x_lo);
			log2_x.lo += log2_x_hi.lo + static_cast<long double>(log2_r.lo);

			const long double y6 = exp * 0x1.0p6L;

			LongDoublePair y6_log2_x = exact_mult(y6, log2_x.hi);
			y6_log2_x.lo			 = support::multiply_add(y6, log2_x.lo, y6_log2_x.lo);

			long double scale = 1.0L;

			if (FPBits_t(y6_log2_x.hi).abs().get_val() >= kUpperY6Bound)
			{
				if (FPBits_t(y6_log2_x.hi).sign().is_pos())
				{
					scale = kScaleUp;
					y6_log2_x.hi -= 512.0L * 64.0L;
					if (y6_log2_x.hi > kOverflowY6HiClamp) { y6_log2_x.hi = kOverflowY6HiClamp; }
				}
				else
				{
					scale = kScaleDown;
					y6_log2_x.hi += 512.0L * 64.0L;
					if (y6_log2_x.hi < kUnderflowY6HiClamp)
					{
						y6_log2_x.hi = kUnderflowY6HiClamp;
						// If hi is clamped, a huge tail would dominate lo6 and reconstruct a spurious normal.
						y6_log2_x.lo = 0.0L;
					}
				}
			}

			const long double hm	 = nearest_integer(y6_log2_x.hi);
			const long double lo6_hi = y6_log2_x.hi - hm;
			const long double lo6	 = lo6_hi + y6_log2_x.lo;

			const int hm_i				 = static_cast<int>(hm);
			const unsigned idx_y		 = static_cast<unsigned>(hm_i) & 0x3fU;
			const int exp_hi			 = hm_i >> 6;
			const long double exp_factor = support::helpers::internal_ldexp(1.0L, exp_hi);
			const auto & mid			 = tables::EXP2_MID1.at(static_cast<std::size_t>(idx_y));
			const long double exp2_hm_hi = exp_factor * static_cast<long double>(mid.hi);
			const long double exp2_hm_lo = idx_y != 0U ? exp_factor * static_cast<long double>(mid.mid) : 0.0L;

			const long double lo6_sqr = lo6 * lo6;
			const long double d0	  = support::multiply_add(lo6, tables::POW_EXP2_COEFFS[2], tables::POW_EXP2_COEFFS[1]);
			const long double d1	  = support::multiply_add(lo6, tables::POW_EXP2_COEFFS[4], tables::POW_EXP2_COEFFS[3]);
			const long double pp	  = support::polyeval(lo6_sqr, d0, d1, tables::POW_EXP2_COEFFS[5]);

			long double result = support::multiply_add(exp2_hm_hi * lo6, pp, exp2_hm_lo);
			result += exp2_hm_hi;

			long double final = result * scale;
			FPBits_t final_bits(final);
			if (final_bits.sign().is_neg())
			{
				if (final_bits.is_inf())
				{
					support::fenv::set_errno_if_required(ERANGE);
					support::fenv::raise_except_if_required(FE_OVERFLOW);
					final = FPBits_t::inf(types::Sign::POS).get_val();
				}
				else if (!final_bits.is_zero() && final_bits.abs().uintval() < FPBits_t::min_subnormal().uintval())
				{
					support::fenv::set_errno_if_required(ERANGE);
					support::fenv::raise_except_if_required(FE_UNDERFLOW);
					final = 0.0L;
				}
				else { final = final_bits.abs().get_val(); }
			}
			else if (scale == kScaleDown && final_bits.is_finite() && !final_bits.is_zero() && final_bits.abs().uintval() < FPBits_t::min_subnormal().uintval())
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
