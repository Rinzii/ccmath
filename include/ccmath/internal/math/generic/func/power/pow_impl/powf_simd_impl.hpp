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

// Vectorized single precision pow. The common path mirrors the scalar powf_impl
// kernel operation for operation in double precision lanes, so every lane that
// resolves through it is bit identical to ccm::powf and therefore correctly
// rounded under the default rounding mode. Lanes that are exceptional, fail the
// Ziv test, or land on overflow/underflow are recomputed with the scalar kernel,
// which keeps the correctly rounded result and the matching fenv side effects.

#include "ccmath/internal/math/generic/builtins/basic/fma.hpp"
#include "ccmath/internal/math/generic/func/power/pow_impl/powf_data.hpp"
#include "ccmath/internal/math/generic/func/power/pow_impl/powf_impl.hpp"
#include "ccmath/internal/math/runtime/pp/pp.hpp"
#include "ccmath/internal/support/common_math_constants.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"

#include <array>
#include <cerrno>
#include <cstdint>
#include <limits>

namespace ccm::gen::impl
{
	namespace simd_detail
	{
		// log2(1 + dx)/dx degree-5 approximation, identical to the scalar powf kernel.
		inline constexpr std::array<double, 6> kLog2Coeffs = { 0x1.71547652b82fep0,	  -0x1.71547652b7a07p-1, 0x1.ec709dc458db1p-2,
															   -0x1.715479c2266c9p-2, 0x1.2776ae1ddf8fp-2,	 -0x1.e7b2178870157p-3 };

		// 2^(x/64) degree-5 approximation, identical to the scalar powf kernel.
		inline constexpr std::array<double, 6> kExp2Coeffs = {
			0x1p0, 0x1.62e42fefa39efp-7, 0x1.ebfbdff82a23ap-15, 0x1.c6b08d7076268p-23, 0x1.3b2ad33f8b48bp-31, 0x1.5d870c4d84445p-40
		};

		template <typename Abi>
		[[nodiscard]] CCM_ALWAYS_INLINE pp::basic_simd<double, Abi> v_nearest_integer(pp::basic_simd<double, Abi> const & x) noexcept
		{
			using DVec = pp::basic_simd<double, Abi>;
			const DVec off(0x1.0p52);
			const DVec pos = (x + off) - off;
			const DVec neg = (x - off) + off;
			const DVec r   = pp::simd_select(x < DVec(0.0), neg, pos);
			// Only the round-to-nearest, tie-to-even branch is needed: the SIMD path runs under FE_TONEAREST.
			return pp::simd_select(pp::abs(x) < DVec(0x1p53), r, x);
		}
	} // namespace simd_detail

	// x^y for a vector of single precision lanes.
	template <typename Abi>
	[[nodiscard]] inline pp::basic_simd<float, Abi> powf_simd(pp::basic_simd<float, Abi> x, pp::basic_simd<float, Abi> y) noexcept
	{
		using FVec = pp::basic_simd<float, Abi>;
		using DVec = pp::basic_simd<double, Abi>;
		using U32  = pp::basic_simd<std::uint32_t, Abi>;
		using U64  = pp::basic_simd<std::uint64_t, Abi>;
		using I64  = pp::basic_simd<std::int64_t, Abi>;

		constexpr auto N = static_cast<int>(FVec::size());

		// Without fused multiply-add the scalar kernel takes a different reduction path,
		// so there is nothing to gain from a non-bit-identical vector path. Fall back.
		if constexpr (!ccm::builtin::target_cpu_has_fma)
		{
			FVec out{};
			for (int i = 0; i < N; ++i) { out[i] = ccm::gen::impl::powf_impl(x[i], y[i]); }
			return out;
		}
		else
		{
			namespace cst = ccm::support::constants;

			const U32 xu_orig = pp::simd_bit_cast<std::uint32_t>(x);
			const U32 yu_orig = pp::simd_bit_cast<std::uint32_t>(y);

			// Exceptional gates copied verbatim from handle_exceptional_cases so that every
			// lane the scalar would divert is recomputed scalar side.
			const U32 y_abs = yu_orig & U32(0x7fff'ffffU);

			auto special = ((y_abs & U32(0x0007'ffffU)) == U32(0)) | (y_abs > U32(0x4f17'0000U));
			special		 = special | ((xu_orig & U32(0x801f'ffffU)) == U32(0)) | (xu_orig >= U32(0x7f80'0000U)) | (xu_orig < U32(0x0080'0000U));

			// Replace exceptional lanes with a benign value so the unconditional vector main path cannot
			// raise spurious floating point exceptions on them. The originals are kept for the scalar
			// fallback, which is what actually computes those lanes.
			const FVec xs = pp::simd_select(special, FVec(1.0F), x);
			const FVec ys = pp::simd_select(special, FVec(1.0F), y);

			const U32 xu = pp::simd_bit_cast<std::uint32_t>(xs);

			// log2(x) = e_x + log2(m_x), with m_x in [1, 2) indexed by the top 7 mantissa bits.
			const U32 ex_raw  = xu >> U32(23);
			const DVec e_x	  = DVec(ex_raw) - DVec(127.0);
			const U32 x_mant  = xu & U32(0x007f'ffffU);
			const U32 idx_x_u = x_mant >> U32(16);
			const FVec m_x	  = pp::simd_bit_cast<float>(x_mant | U32(0x3f80'0000U));

			// Per-lane table gathers.
			const FVec r_tab([&](auto i) { return cst::R[static_cast<std::size_t>(idx_x_u[i])]; });
			const DVec log2r([&](auto i) { return cst::LOG2_R[static_cast<std::size_t>(idx_x_u[i])]; });
			const DVec lr_hi([&](auto i) { return internal::impl::LOG2_R_TD[static_cast<std::size_t>(idx_x_u[i])].hi; });
			const DVec lr_mid([&](auto i) { return internal::impl::LOG2_R_TD[static_cast<std::size_t>(idx_x_u[i])].mid; });

			// dx = m_x * R[idx] - 1, computed in float then widened (exact).
			const DVec dx  = DVec(pp::fma(m_x, r_tab, FVec(-1.0F)));
			const DVec dx2 = dx * dx;
			const DVec c0  = pp::fma(dx, DVec(simd_detail::kLog2Coeffs[1]), DVec(simd_detail::kLog2Coeffs[0]));
			const DVec c1  = pp::fma(dx, DVec(simd_detail::kLog2Coeffs[3]), DVec(simd_detail::kLog2Coeffs[2]));
			const DVec c2  = pp::fma(dx, DVec(simd_detail::kLog2Coeffs[5]), DVec(simd_detail::kLog2Coeffs[4]));
			const DVec p   = pp::fma(dx2, pp::fma(dx2, c2, c1), c0); // polyeval(dx2, c0, c1, c2)
			const DVec s   = pp::fma(dx, p, log2r + e_x);

			const DVec y6	  = DVec(ys * FVec(0x1.0p6F));
			const DVec hm	  = simd_detail::v_nearest_integer(s * y6);
			const DVec lo6_hi = pp::fma(y6, e_x + lr_hi, -hm);
			const DVec lo6	  = pp::fma(y6, pp::fma(dx, p, lr_mid), lo6_hi);

			// Clamp 2^(hi+mid) exponent into double range. Out of range lanes round at the cast.
			I64 hm_i(hm);
			hm_i = pp::simd_select(hm_i > I64(1 << 15), I64(1 << 15), hm_i);
			hm_i = pp::simd_select(hm_i < I64(-(1 << 15)), I64(-(1 << 15)), hm_i);

			const I64 idx_y_i  = hm_i & I64(0x3f);
			const I64 exp_hi_i = (hm_i >> I64(6)) << I64(52);
			const DVec emid_hi([&](auto i) { return cst::EXP2_MID1[static_cast<std::size_t>(idx_y_i[i])].hi; });
			const I64 exp_mid_i	   = pp::simd_bit_cast<std::int64_t>(emid_hi);
			const DVec exp2_hi_mid = pp::simd_bit_cast<double>(exp_hi_i + exp_mid_i);

			// 2^lo via degree-5 polynomial.
			const DVec lo6_sqr = lo6 * lo6;
			const DVec d0	   = pp::fma(lo6, DVec(simd_detail::kExp2Coeffs[1]), DVec(simd_detail::kExp2Coeffs[0]));
			const DVec d1	   = pp::fma(lo6, DVec(simd_detail::kExp2Coeffs[3]), DVec(simd_detail::kExp2Coeffs[2]));
			const DVec d2	   = pp::fma(lo6, DVec(simd_detail::kExp2Coeffs[5]), DVec(simd_detail::kExp2Coeffs[4]));
			const DVec pp_poly = pp::fma(lo6_sqr, pp::fma(lo6_sqr, d2, d1), d0);

			const DVec r = pp_poly * exp2_hi_mid;

			// Ziv accuracy test, mirroring the scalar kernel exactly. The vector interior reproduces the
			// scalar computation operation for operation, so reusing the same error_tolerance makes the
			// resolution decision and the returned r_upper bit identical to powf_impl. Lanes the scalar
			// would refine through its double-double path fail here too and are recomputed scalar side.
			const U64 r_u = pp::simd_bit_cast<std::uint64_t>(r);
			const FVec r_upper(pp::simd_bit_cast<double>(r_u + U64(internal::impl::error_tolerance)));
			const FVec r_lower(pp::simd_bit_cast<double>(r_u - U64(internal::impl::error_tolerance)));

			const auto ziv_unresolved = (r_upper != r_lower);

			const auto needs_scalar = special | ziv_unresolved;

			FVec result = r_upper;
			if (pp::any_of(needs_scalar))
			{
				for (int i = 0; i < N; ++i)
				{
					if (needs_scalar[i]) { result[i] = ccm::gen::impl::powf_impl(x[i], y[i]); }
				}
			}

			// Overflow/underflow on the fast path raise the same fenv side effects the scalar kernel does.
			// Exceptional and Ziv-hard lanes already raised theirs scalar side. The flags are sticky, so a
			// single raise per vector call matches the observable state of the per-lane scalar raises.
			const auto fast_lane = ~needs_scalar;
			const FVec r_abs	 = pp::abs(r_upper);
			if (pp::any_of(fast_lane & (r_abs == FVec(std::numeric_limits<float>::infinity()))))
			{
				ccm::support::fenv::set_errno_if_required(ERANGE);
				ccm::support::fenv::raise_except_if_required(FE_OVERFLOW);
			}
			if (pp::any_of(fast_lane & (r_upper == FVec(0.0F))))
			{
				ccm::support::fenv::set_errno_if_required(ERANGE);
				ccm::support::fenv::raise_except_if_required(FE_UNDERFLOW);
			}
			return result;
		}
	}

} // namespace ccm::gen::impl
