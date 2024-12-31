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

#pragma once

#include <type_traits>

#include "ccmath/internal/math/generic/builtins/basic/fma.hpp"
#include "ccmath/internal/math/generic/func/power/pow_impl/powf_data.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/common_math_constants.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/support/fp/nearest_integer.hpp"
#include "ccmath/internal/support/fp/ops/add.hpp"
#include "ccmath/internal/support/fp/ops/mul.hpp"
#include "ccmath/internal/types/double_double.hpp"
#include "ccmath/internal/types/triple_double.hpp"

namespace ccm::gen::impl
{
	namespace internal::impl
	{
		using namespace ccm::type;

#ifdef CCMATH_TARGET_CPU_HAS_FMA
		constexpr std::uint64_t error_tolerance = 64;
#else
		constexpr std::uint64_t error_tolerance = 128;
#endif

		constexpr bool is_odd_integer(float val) noexcept
		{
			using FPBits_t					= support::fp::FPBits<float>;
			const std::uint32_t x_u			= ccm::support::bit_cast<std::uint32_t>(val);
			const auto x_e					= static_cast<std::int32_t>((x_u & FPBits_t::exponent_mask) >> FPBits_t::fraction_length);
			const std::int32_t lsb			= support::countr_zero(x_u | FPBits_t::exponent_mask);
			constexpr int32_t unit_exponent = FPBits_t::exponent_bias + static_cast<std::int32_t>(FPBits_t::fraction_length);
			return (x_e + lsb == unit_exponent);
		}

		constexpr bool is_integer(float val)
		{
			using FPBits_t						 = support::fp::FPBits<float>;
			const std::uint32_t x_u				 = ccm::support::bit_cast<uint32_t>(val);
			const auto x_e						 = static_cast<std::int32_t>((x_u & FPBits_t::exponent_mask) >> FPBits_t::fraction_length);
			const std::int32_t lsb				 = support::countr_zero(x_u | FPBits_t::exponent_mask);
			constexpr std::int32_t unit_exponent = FPBits_t::exponent_bias + static_cast<std::int32_t>(FPBits_t::fraction_length);
			return (x_e + lsb >= unit_exponent);
		}

		constexpr bool larger_exponent(double a, double b)
		{
			using DoubleBits = support::fp::FPBits<double>;
			return DoubleBits(a).get_biased_exponent() >= DoubleBits(b).get_biased_exponent();
		}

		constexpr double powf_double_double(int idx_x, double dx, double y6, double lo6_hi, const DoubleDouble & exp2_hi_mid)
		{
			// using DoubleBits = support::fp::FPBits<double>;

			// Perform a second range reduction step:
			//   idx2 = round(2^14 * (dx  + 2^-8)) = round ( dx * 2^14 + 2^6)
			//   dx2 = (1 + dx) * r2 - 1
			// Output range:
			//   -0x1.3ffcp-15 <= dx2 <= 0x1.3e3dp-15
			int idx2   = static_cast<int>(support::fp::nearest_integer(support::multiply_add(dx, 0x1.0p14, 0x1.0p6)));
			double dx2 = support::multiply_add(1.0 + dx, support::constants::R2.at(idx2), -1.0); // Exact

			// Degree-5 polynomial approximation of log2(1 + x)/x in double-double
			// precision.  Generate by Solya with:
			// > P = fpminimax(log2(1 + x)/x, 5, [|DD...|],
			//                 [-0x1.3ffcp-15, 0x1.3e3dp-15]);
			// > dirtyinfnorm(log2(1 + x)/x - P, [-0x1.3ffcp-15, 0x1.3e3dp-15]);
			// 0x1.8be5...p-96.
			constexpr DoubleDouble COEFFS[] = {
				{0x1.777d0ffda25ep-56, 0x1.71547652b82fep0},	{-0x1.777d101cf0a84p-57, -0x1.71547652b82fep-1}, {0x1.ce04b5140d867p-56, 0x1.ec709dc3a03fdp-2},
				{0x1.137b47e635be5p-56, -0x1.71547652b82fbp-2}, {-0x1.b5a30b3bdb318p-58, 0x1.2776c516a92a2p-2},	 {0x1.2d2fbd081e657p-57, -0x1.ec70af1929ca6p-3},
			};

			/*
			DoubleDouble dx_dd({0.0, dx2});
			DoubleDouble p = support::polyeval(dx_dd, COEFFS[0], COEFFS[1], COEFFS[2], COEFFS[3], COEFFS[4], COEFFS[5]);
			// log2(1 + dx2) ~ dx2 * P(dx2)
			DoubleDouble log2_x_lo = support::quick_mult(dx2, p);
			// Lower parts of (e_x - log2(r1)) of the first range reduction constant
			DoubleDouble log2_x_mid({LOG2_R_TD[idx_x].lo, LOG2_R_TD[idx_x].mid});
			// -log2(r2) + lower part of (e_x - log2(r1))
			DoubleDouble log2_x_m = support::add(LOG2_R2_DD[idx2], log2_x_mid);
			// log2(1 + dx2) - log2(r2) + lower part of (e_x - log2(r1))
			// Since we don't know which one has larger exponent to apply Fast2Sum
			// algorithm, we need to check them before calling double-double addition.
			DoubleDouble log2_x = larger_exponent(log2_x_m.hi, log2_x_lo.hi) ? support::add(log2_x_m, log2_x_lo) : support::add(log2_x_lo, log2_x_m);
			DoubleDouble lo6_hi_dd({0.0, lo6_hi});
			// 2^6 * y * (log2(1 + dx2) - log2(r2) + lower part of (e_x - log2(r1)))
			DoubleDouble prod = support::quick_mult(y6, log2_x);
			// 2^6 * (y * log2(x) - (hi + mid)) = 2^6 * lo
			DoubleDouble lo6 = larger_exponent(prod.hi, lo6_hi) ? support::add(prod, lo6_hi_dd) : support::add(lo6_hi_dd, prod);
			*/
			return 0.0;
		}

		constexpr float powf_impl([[maybe_unused]] float base, [[maybe_unused]] float exp) noexcept
		{
			// TODO: Implement this.
			return 0;
		}
	} // namespace internal::impl

	constexpr float powf_impl(float base, float exp) noexcept
	{
		return 0;
		// return internal::impl::powf_impl(base, exp);
	}

} // namespace ccm::gen::impl
