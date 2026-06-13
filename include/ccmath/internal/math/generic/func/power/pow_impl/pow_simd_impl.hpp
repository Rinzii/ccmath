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

// Vectorized double precision pow. The in-range path of the scalar kernel is branch free
// double-double arithmetic ending in pow_double_double, which is correctly rounded by
// construction (no Ziv retry). This kernel replays that exact computation in vector lanes,
// so every lane it resolves is bit identical to the scalar pow_impl. Lanes the scalar
// would divert (special values, integer and half-integer exponents, subnormal or negative
// bases, huge exponents, and the over/underflow scale region) are recomputed with the
// scalar kernel, which also owns the errno and floating point exception side effects.

#include "ccmath/internal/math/generic/func/power/pow_impl/pow_impl.hpp"
#include "ccmath/internal/math/generic/func/power/pow_impl/powf_simd_impl.hpp"
#include "ccmath/internal/support/common_math_constants.hpp"

#include <cstddef>
#include <cstdint>

namespace ccm::gen::impl
{
	namespace simd_detail
	{
		// Vector double-double pair. Each helper below replays the matching ccm::types
		// primitive lane for lane, so results are bit identical to the scalar kernel.
		template <typename Abi>
		struct VDD
		{
			pp::basic_simd<double, Abi> hi;
			pp::basic_simd<double, Abi> lo;
		};

		// Dekker's FastTwoSum. Same assumption as types::exact_add: |a| >= |b| or a == 0.
		template <typename Abi>
		CCM_ALWAYS_INLINE VDD<Abi> v_exact_add(pp::basic_simd<double, Abi> const & a, pp::basic_simd<double, Abi> const & b) noexcept
		{
			using DVec	  = pp::basic_simd<double, Abi>;
			const DVec hi = a + b;
			const DVec t  = hi - a;
			return { hi, b - t };
		}

		// types::add(DoubleDouble, DoubleDouble). Assumption: |a.hi| >= |b.hi|.
		template <typename Abi>
		CCM_ALWAYS_INLINE VDD<Abi> v_add(VDD<Abi> const & a, VDD<Abi> const & b) noexcept
		{
			const VDD<Abi> r = v_exact_add(a.hi, b.hi);
			return v_exact_add(r.hi, r.lo + (a.lo + b.lo));
		}

		// Veltkamp's splitting.
		template <typename Abi>
		CCM_ALWAYS_INLINE VDD<Abi> v_split(pp::basic_simd<double, Abi> const & a) noexcept
		{
			using DVec = pp::basic_simd<double, Abi>;
			const DVec c(0x1.0p27 + 1.0);
			const DVec t1 = c * a;
			const DVec t2 = a - t1;
			const DVec hi = t1 + t2;
			return { hi, a - hi };
		}

		// Dekker's product, mirroring types::exact_mult.
		template <typename Abi>
		CCM_ALWAYS_INLINE VDD<Abi> v_exact_mult(pp::basic_simd<double, Abi> const & a, pp::basic_simd<double, Abi> const & b) noexcept
		{
			using DVec		  = pp::basic_simd<double, Abi>;
			const VDD<Abi> as = v_split(a);
			const VDD<Abi> bs = v_split(b);
			const DVec hi	  = a * b;
			const DVec t1	  = as.hi * bs.hi - hi;
			const DVec t2	  = as.hi * bs.lo + t1;
			const DVec t3	  = as.lo * bs.hi + t2;
			return { hi, as.lo * bs.lo + t3 };
		}

		// types::quick_mult(double, DoubleDouble).
		template <typename Abi>
		CCM_ALWAYS_INLINE VDD<Abi> v_quick_mult(pp::basic_simd<double, Abi> const & a, VDD<Abi> const & b) noexcept
		{
			VDD<Abi> r = v_exact_mult(a, b.hi);
			r.lo	   = pp::fma(a, b.lo, r.lo);
			return r;
		}

		// types::quick_mult(DoubleDouble, DoubleDouble).
		template <typename Abi>
		CCM_ALWAYS_INLINE VDD<Abi> v_quick_mult(VDD<Abi> const & a, VDD<Abi> const & b) noexcept
		{
			using DVec	  = pp::basic_simd<double, Abi>;
			VDD<Abi> r	  = v_exact_mult(a.hi, b.hi);
			const DVec t1 = pp::fma(a.hi, b.lo, r.lo);
			r.lo		  = pp::fma(a.lo, b.hi, t1);
			return r;
		}

		// support::multiply_add<DoubleDouble>, the polyeval step: add(z, quick_mult(x, y)).
		template <typename Abi>
		CCM_ALWAYS_INLINE VDD<Abi> v_multiply_add(VDD<Abi> const & x, VDD<Abi> const & y, VDD<Abi> const & z) noexcept
		{ return v_add(z, v_quick_mult(x, y)); }

		template <typename Abi>
		CCM_ALWAYS_INLINE pp::basic_simd<std::uint64_t, Abi> v_biased_exponent(pp::basic_simd<double, Abi> const & a) noexcept
		{
			using U64 = pp::basic_simd<std::uint64_t, Abi>;
			return (pp::simd_bit_cast<std::uint64_t>(a) >> U64(52)) & U64(0x7ff);
		}

		template <typename Abi>
		CCM_ALWAYS_INLINE VDD<Abi> v_select(pp::basic_simd_mask<8, Abi> const & mask, VDD<Abi> const & a, VDD<Abi> const & b) noexcept
		{ return { pp::simd_select(mask, a.hi, b.hi), pp::simd_select(mask, a.lo, b.lo) }; }

		template <typename Abi>
		CCM_ALWAYS_INLINE VDD<Abi> v_broadcast(types::DoubleDouble const & c) noexcept
		{
			using DVec = pp::basic_simd<double, Abi>;
			return { DVec(c.hi), DVec(c.lo) };
		}
	} // namespace simd_detail

	// x^y for a vector of double precision lanes.
	template <typename Abi>
	[[nodiscard]] inline pp::basic_simd<double, Abi> pow_simd(pp::basic_simd<double, Abi> x, pp::basic_simd<double, Abi> y) noexcept
	{
		using DVec = pp::basic_simd<double, Abi>;
		using U64  = pp::basic_simd<std::uint64_t, Abi>;
		using I64  = pp::basic_simd<std::int64_t, Abi>;
		using VDD  = simd_detail::VDD<Abi>;

		constexpr auto N = static_cast<int>(DVec::size());

		// The scalar kernel takes a different reduction without fused multiply-add, so a
		// bit-identical vector replay is only possible on FMA targets. Fall back otherwise.
		if constexpr (!ccm::builtin::target_cpu_has_fma)
		{
			DVec out{};
			for (int i = 0; i < N; ++i) { out[i] = ccm::gen::impl::pow_impl(x[i], y[i]); }
			return out;
		}
		else
		{
			namespace cst = ccm::support::constants;
			namespace sd  = simd_detail;
			using internal::impl::LOG2_R2_DD;
			using internal::impl::LOG2_R_TD;
			using internal::impl::POW_DD_EXP2_COEFFS;
			using internal::impl::POW_DD_LOG2P_COEFFS;
			using internal::impl::POW_LOG2_COEFFS;

			const U64 xu	= pp::simd_bit_cast<std::uint64_t>(x);
			const U64 yu	= pp::simd_bit_cast<std::uint64_t>(y);
			const U64 x_abs = xu & U64(0x7fff'ffff'ffff'ffffULL);
			const U64 y_abs = yu & U64(0x7fff'ffff'ffff'ffffULL);

			// Every case the scalar pow_impl diverts before reaching the in-range kernel:
			// zero exponent, base one, non-finite operands, zero, negative, or subnormal
			// bases, integer and half-integer exponents (the exact ipow / sqrt paths), and
			// huge exponents that take the clamped over/underflow route.
			const DVec two_y	   = y * DVec(2.0);
			const auto is_half_int = (sd::v_nearest_integer(two_y) == two_y) & (pp::abs(two_y) <= DVec(2048.0));

			auto special = (y_abs == U64(0)) | (xu == U64(0x3ff0'0000'0000'0000ULL)) | (x_abs >= U64(0x7ff0'0000'0000'0000ULL)) |
						   (y_abs >= U64(0x7ff0'0000'0000'0000ULL)) | (x_abs == U64(0)) | ((xu & U64(0x8000'0000'0000'0000ULL)) != U64(0)) |
						   (x_abs < U64(0x0010'0000'0000'0000ULL)) | is_half_int | (y_abs > U64(0x43d7'4910'd52d'3052ULL));

			// Benign stand-ins keep the unconditional vector arithmetic clean of spurious
			// floating point exceptions on diverted lanes. The original values are kept for
			// the scalar fallback, which is what actually computes those lanes.
			const DVec xs = pp::simd_select(special, DVec(1.5), x);
			const DVec ys = pp::simd_select(special, DVec(0.375), y);

			// Range reduction, replaying pow_kernel with sign = 0 and no subnormal adjust
			// (negative and subnormal bases are already diverted).
			const U64 xu_s	 = pp::simd_bit_cast<std::uint64_t>(xs);
			const DVec e_x	 = DVec(pp::simd_bit_cast<std::int64_t>(xu_s >> U64(52)) - I64(1023));
			const U64 x_mant = xu_s & U64(0x000f'ffff'ffff'ffffULL);
			const U64 idx_x	 = x_mant >> U64(45);
			const DVec m_x	 = pp::simd_bit_cast<double>(x_mant | U64(0x3ff0'0000'0000'0000ULL));

			const DVec rd([&](auto i) { return cst::RD[static_cast<std::size_t>(idx_x[i])]; });
			const DVec lr_hi([&](auto i) { return LOG2_R_TD[static_cast<std::size_t>(idx_x[i])].hi; });
			const DVec lr_mid([&](auto i) { return LOG2_R_TD[static_cast<std::size_t>(idx_x[i])].mid; });
			const DVec lr_lo([&](auto i) { return LOG2_R_TD[static_cast<std::size_t>(idx_x[i])].lo; });

			const DVec dx	= pp::fma(rd, m_x, DVec(-1.0));
			const VDD dx_c0 = sd::v_exact_mult(DVec(POW_LOG2_COEFFS[0]), dx);

			const DVec dx2 = dx * dx;
			const DVec c0  = pp::fma(dx, DVec(POW_LOG2_COEFFS[2]), DVec(POW_LOG2_COEFFS[1]));
			const DVec c1  = pp::fma(dx, DVec(POW_LOG2_COEFFS[4]), DVec(POW_LOG2_COEFFS[3]));
			const DVec c2  = pp::fma(dx, DVec(POW_LOG2_COEFFS[6]), DVec(POW_LOG2_COEFFS[5]));
			const DVec p   = pp::fma(dx2, pp::fma(dx2, c2, c1), c0);

			const DVec log2_x_hi_part = e_x + lr_hi;
			const VDD log2_x_hi		  = sd::v_exact_add(log2_x_hi_part, dx_c0.hi);
			const DVec log2_x_lo	  = pp::fma(dx2, p, dx_c0.lo + lr_mid);
			VDD log2_x				  = sd::v_exact_add(log2_x_hi.hi, log2_x_lo);
			log2_x.lo				  = log2_x.lo + (log2_x_hi.lo + lr_lo);

			const DVec y6 = ys * DVec(0x1.0p6);
			VDD y6_log2	  = sd::v_exact_mult(y6, log2_x.hi);
			y6_log2.lo	  = pp::fma(y6, log2_x.lo, y6_log2.lo);

			// Lanes in the clamped over/underflow region keep the scalar fast path, which
			// saturates cleanly and raises the matching errno and exception side effects.
			const auto needs_scalar = special | (pp::abs(y6_log2.hi) >= DVec(512.0 * 64.0));

			// pow_double_double replayed in vector lanes. Zeroing y6 on diverted lanes keeps
			// hm small so the integer conversion below stays defined and flag clean.
			const DVec y6_dd = pp::simd_select(needs_scalar, DVec(0.0), y6);

			const DVec idx2_d = sd::v_nearest_integer(pp::fma(dx, DVec(0x1.0p14), DVec(0x1.0p6)));
			const I64 idx2_i(idx2_d);
			const DVec r2([&](auto i) { return cst::R2[static_cast<std::size_t>(idx2_i[i])]; });

			const VDD one_plus_dx = sd::v_exact_add(DVec(1.0), dx);
			const VDD r2_prod	  = sd::v_quick_mult(r2, one_plus_dx);
			const DVec dx2_hi	  = r2_prod.hi - DVec(1.0); // Exact by Sterbenz, r2_prod.hi ~ 1
			const VDD dx2_dd	  = sd::v_exact_add(dx2_hi, r2_prod.lo);

			VDD p_dd = sd::v_broadcast<Abi>(POW_DD_LOG2P_COEFFS[5]);
			p_dd	 = sd::v_multiply_add(dx2_dd, p_dd, sd::v_broadcast<Abi>(POW_DD_LOG2P_COEFFS[4]));
			p_dd	 = sd::v_multiply_add(dx2_dd, p_dd, sd::v_broadcast<Abi>(POW_DD_LOG2P_COEFFS[3]));
			p_dd	 = sd::v_multiply_add(dx2_dd, p_dd, sd::v_broadcast<Abi>(POW_DD_LOG2P_COEFFS[2]));
			p_dd	 = sd::v_multiply_add(dx2_dd, p_dd, sd::v_broadcast<Abi>(POW_DD_LOG2P_COEFFS[1]));
			p_dd	 = sd::v_multiply_add(dx2_dd, p_dd, sd::v_broadcast<Abi>(POW_DD_LOG2P_COEFFS[0]));

			const VDD log2_1p = sd::v_quick_mult(dx2_dd, p_dd);

			// Lower-order parts of (e_x - log2(r1)) and -log2(r2). LOG2_R2_DD is stored {lo, hi}.
			const VDD log2_x_mid{ lr_mid, lr_lo };
			const DVec lr2_lo([&](auto i) { return LOG2_R2_DD[static_cast<std::size_t>(idx2_i[i])].lo; });
			const DVec lr2_hi([&](auto i) { return LOG2_R2_DD[static_cast<std::size_t>(idx2_i[i])].hi; });
			const VDD log2_r2_dd{ lr2_lo, lr2_hi };
			const VDD log2_x_m = sd::v_add(log2_r2_dd, log2_x_mid);

			// The two Fast2Sum orderings the scalar resolves with larger_exponent branches.
			const auto m1		 = sd::v_biased_exponent(log2_x_m.hi) >= sd::v_biased_exponent(log2_1p.hi);
			const VDD log2_x_low = sd::v_add(sd::v_select(m1, log2_x_m, log2_1p), sd::v_select(m1, log2_1p, log2_x_m));

			// e_x + lr_hi as an exact_add so the rounding residual of a far-from-one base survives
			// into the accurate path, matching the scalar pow_double_double. The bare
			// {log2_x_hi_part, 0} pair used previously dropped it and drifted up to ~1.5 ULP.
			const VDD hi_dd		  = sd::v_exact_add(e_x, lr_hi);
			const auto m2		  = sd::v_biased_exponent(hi_dd.hi) >= sd::v_biased_exponent(log2_x_low.hi);
			const VDD log2_x_full = sd::v_add(sd::v_select(m2, hi_dd, log2_x_low), sd::v_select(m2, log2_x_low, hi_dd));

			const VDD y6_log2_x = sd::v_quick_mult(y6_dd, log2_x_full);

			const DVec hm	  = sd::v_nearest_integer(y6_log2_x.hi);
			const DVec lo6_hi = y6_log2_x.hi - hm; // Exact, |lo6_hi| <= 0.5
			const VDD lo6	  = sd::v_exact_add(lo6_hi, y6_log2_x.lo);

			const I64 hm_i(hm);
			const I64 idx_y		= hm_i & I64(0x3f);
			const I64 exp2_hi_i = (hm_i >> I64(6)) << I64(52);
			const I64 mid_hi_i([&](auto i) { return support::bit_cast<std::int64_t>(cst::EXP2_MID1[static_cast<std::size_t>(idx_y[i])].hi); });
			const I64 mid_lo_i([&](auto i) { return support::bit_cast<std::int64_t>(cst::EXP2_MID1[static_cast<std::size_t>(idx_y[i])].mid); });

			const DVec exp2_hm_hi = pp::simd_bit_cast<double>(exp2_hi_i + mid_hi_i);
			const DVec exp2_hm_lo = pp::simd_bit_cast<double>(pp::simd_select(idx_y != I64(0), exp2_hi_i + mid_lo_i, I64(0)));
			const VDD exp2_hm{ exp2_hm_hi, exp2_hm_lo };

			VDD exp2_poly = sd::v_broadcast<Abi>(POW_DD_EXP2_COEFFS[9]);
			exp2_poly	  = sd::v_multiply_add(lo6, exp2_poly, sd::v_broadcast<Abi>(POW_DD_EXP2_COEFFS[8]));
			exp2_poly	  = sd::v_multiply_add(lo6, exp2_poly, sd::v_broadcast<Abi>(POW_DD_EXP2_COEFFS[7]));
			exp2_poly	  = sd::v_multiply_add(lo6, exp2_poly, sd::v_broadcast<Abi>(POW_DD_EXP2_COEFFS[6]));
			exp2_poly	  = sd::v_multiply_add(lo6, exp2_poly, sd::v_broadcast<Abi>(POW_DD_EXP2_COEFFS[5]));
			exp2_poly	  = sd::v_multiply_add(lo6, exp2_poly, sd::v_broadcast<Abi>(POW_DD_EXP2_COEFFS[4]));
			exp2_poly	  = sd::v_multiply_add(lo6, exp2_poly, sd::v_broadcast<Abi>(POW_DD_EXP2_COEFFS[3]));
			exp2_poly	  = sd::v_multiply_add(lo6, exp2_poly, sd::v_broadcast<Abi>(POW_DD_EXP2_COEFFS[2]));
			exp2_poly	  = sd::v_multiply_add(lo6, exp2_poly, sd::v_broadcast<Abi>(POW_DD_EXP2_COEFFS[1]));
			exp2_poly	  = sd::v_multiply_add(lo6, exp2_poly, sd::v_broadcast<Abi>(POW_DD_EXP2_COEFFS[0]));

			const VDD rr = sd::v_quick_mult(exp2_hm, exp2_poly);

			// A single binary64 addition of the faithful double-double rounds once, exactly
			// as the scalar reconstruction does.
			DVec result = rr.hi + rr.lo;

			if (pp::any_of(needs_scalar))
			{
				for (int i = 0; i < N; ++i)
				{
					if (needs_scalar[i]) { result[i] = ccm::gen::impl::pow_impl(x[i], y[i]); }
				}
			}
			return result;
		}
	}

} // namespace ccm::gen::impl
