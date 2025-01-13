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

#include "ccmath/internal/math/generic/builtins/power/sqrt.hpp"
#include "ccmath/internal/math/generic/func/power/sqrt_gen.hpp"
#include "ccmath/internal/math/runtime/func/power/sqrt_rt.hpp"

#include <optional>

namespace ccm::gen::impl
{
	namespace internal::impl
	{
		using namespace ccm::types;

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
			using DoubleBits = support::fp::FPBits<double>;

			// Perform a second range reduction step:
			//   - First, compute the index for the second reduction:
			//     idx2 = round(2^14 * (dx + 2^-8)) = round(dx * 2^14 + 2^6)
			//   - Then, calculate the reduced value:
			//     dx2 = (1 + dx) * r2 - 1
			//   - The output range of dx2 is bounded by:
			//     -0x1.3ffcp-15 <= dx2 <= 0x1.3e3dp-15
			const int idx2	 = static_cast<int>(support::fp::nearest_integer(support::multiply_add(dx, 0x1.0p14, 0x1.0p6)));
			const double dx2 = support::multiply_add(1.0 + dx, support::constants::R2.at(idx2), -1.0); // Exact

			// Degree-5 polynomial approximation for log2(1 + x)/x in double-double precision:
			//   - Generated using Sollya with the command:
			//     P = fpminimax(log2(1 + x)/x, 5, [|DD...|], [-0x1.3ffcp-15, 0x1.3e3dp-15])
			//   - The maximum error between log2(1 + x)/x and the polynomial approximation is:
			//     dirtyinfnorm(log2(1 + x)/x - P, [-0x1.3ffcp-15, 0x1.3e3dp-15]) = 0x1.8be5...p-96
			//   - The coefficients for the polynomial are stored in the array `COEFFS`.
			constexpr std::array<DoubleDouble, 6> COEFFS = {
				DoubleDouble{0x1.777d0ffda25ep-56, 0x1.71547652b82fep0},	DoubleDouble{-0x1.777d101cf0a84p-57, -0x1.71547652b82fep-1},
				DoubleDouble{0x1.ce04b5140d867p-56, 0x1.ec709dc3a03fdp-2},	DoubleDouble{0x1.137b47e635be5p-56, -0x1.71547652b82fbp-2},
				DoubleDouble{-0x1.b5a30b3bdb318p-58, 0x1.2776c516a92a2p-2}, DoubleDouble{0x1.2d2fbd081e657p-57, -0x1.ec70af1929ca6p-3},
			};

			const DoubleDouble dx_dd({0.0, dx2});
			const DoubleDouble p = support::polyeval(dx_dd, COEFFS[0], COEFFS[1], COEFFS[2], COEFFS[3], COEFFS[4], COEFFS[5]);

			// log2(1 + dx2) ~ dx2 * P(dx2)
			const DoubleDouble log2_x_lo = quick_mult(dx2, p);

			// Lower parts of (e_x - log2(r1)) of the first range reduction constant
			const DoubleDouble log2_x_mid({LOG2_R_TD.at(idx_x).lo, LOG2_R_TD.at(idx_x).mid});

			// -log2(r2) + lower part of (e_x - log2(r1))
			const DoubleDouble log2_x_m = add(LOG2_R2_DD.at(idx2), log2_x_mid);

			// log2(1 + dx2) - log2(r2) + lower part of (e_x - log2(r1))
			// Since we don't know which one has larger exponent to apply Fast2Sum
			// algorithm, we need to check them before calling double-double addition.
			const DoubleDouble log2_x = larger_exponent(log2_x_m.hi, log2_x_lo.hi) ? add(log2_x_m, log2_x_lo) : add(log2_x_lo, log2_x_m);
			const DoubleDouble lo6_hi_dd({0.0, lo6_hi});

			// 2^6 * y * (log2(1 + dx2) - log2(r2) + lower part of (e_x - log2(r1)))
			const DoubleDouble prod = quick_mult(y6, log2_x);

			// 2^6 * (y * log2(x) - (hi + mid)) = 2^6 * lo
			DoubleDouble lo6								   = larger_exponent(prod.hi, lo6_hi) ? add(prod, lo6_hi_dd) : add(lo6_hi_dd, prod);
			constexpr std::array<DoubleDouble, 10> EXP2_COEFFS = {
				DoubleDouble{0, 0x1p0},
				DoubleDouble{0x1.abc9e3b398024p-62, 0x1.62e42fefa39efp-7},
				DoubleDouble{-0x1.5e43a5429bddbp-69, 0x1.ebfbdff82c58fp-15},
				DoubleDouble{-0x1.d33162491268fp-77, 0x1.c6b08d704a0cp-23},
				DoubleDouble{0x1.4fb32d240a14ep-86, 0x1.3b2ab6fba4e77p-31},
				DoubleDouble{0x1.e84e916be83ep-97, 0x1.5d87fe78a6731p-40},
				DoubleDouble{-0x1.9a447bfddc5e6p-103, 0x1.430912f86bfb8p-49},
				DoubleDouble{-0x1.31a55719de47fp-113, 0x1.ffcbfc588ded9p-59},
				DoubleDouble{-0x1.0ba57164eb36bp-122, 0x1.62c034beb8339p-68},
				DoubleDouble{-0x1.8483eabd9642dp-132, 0x1.b5251ff97bee1p-78},
			};

			DoubleDouble pp		  = support::polyeval(lo6, EXP2_COEFFS[0], EXP2_COEFFS[1], EXP2_COEFFS[2], EXP2_COEFFS[3], EXP2_COEFFS[4], EXP2_COEFFS[5],
													  EXP2_COEFFS[6], EXP2_COEFFS[7], EXP2_COEFFS[8], EXP2_COEFFS[9]);
			const DoubleDouble rr = quick_mult(exp2_hi_mid, pp);

			// Make sure the sum is normalized:
			const DoubleDouble r = exact_add(rr.hi, rr.lo);

			// Round to odd.
			uint64_t r_bits = ccm::support::bit_cast<uint64_t>(r.hi);
			if (CCM_UNLIKELY(((r_bits & 0xfff'ffff) == 0) && (r.lo != 0.0)))
			{
				const types::Sign hi_sign = DoubleBits(r.hi).sign();
				if (const types::Sign lo_sign = DoubleBits(r.lo).sign(); hi_sign == lo_sign) { ++r_bits; }
				else if ((r_bits & DoubleBits::fraction_mask) > 0) { --r_bits; }
			}

			return ccm::support::bit_cast<double>(r_bits);
		}

		constexpr std::optional<float> handle_exceptional_cases(float & x, float & y, support::fp::FPBits<float> & xbits, support::fp::FPBits<float> & ybits,
																const std::uint32_t & x_u, int & exponent, std::uint64_t & sign)
		{
			using FloatBits = support::fp::FPBits<float>;

			const std::uint32_t x_abs = xbits.abs().uintval();
			const std::uint32_t y_u	  = ybits.uintval();
			const std::uint32_t y_abs = ybits.abs().uintval();

			// The single precision number that is closest to 1 is (1 - 2^-24)
			if (CCM_UNLIKELY((y_abs & 0x0007'ffff) == 0 || y_abs > 0x4f170000))
			{
				if (y == 0.0F) { return 1.0F; }

				switch (y_abs)
				{
				case 0x7f800000:
				{ // y is +/-infinity
					if (x_abs > 0x7f80'0000)
					{
						return x; // pow(NaN, +/-inf) = NaN
					}
					if (x_abs == 0x3f80'0000)
					{
						return 1.0F; // pow(+/-1, +/-inf) = 1.0F
					}
					if (x == 0.0F && y_u == 0xff80'0000)
					{
						// pow(+/-0, -inf) = +inf and raise FE_DIVBYZERO
						support::fenv::set_errno_if_required(EDOM);
						support::fenv::raise_except_if_required(FE_DIVBYZERO);
						return FloatBits::inf().get_val();
					}
					return ((x_abs < 0x3f80'0000) == (y_u == 0xff80'0000)) ? FloatBits::inf().get_val() : 0.0F;
				}
				default:
					switch (y_u)
					{
					case 0x3f00'0000: // y = 0.5f
						if (CCM_UNLIKELY(x == 0.0F || x_u == 0xff80'0000)) { return x * x; }
						if (ccm::support::is_constant_evaluated()) { return ccm::gen::sqrt_gen<float>(x); }
						return ccm::rt::sqrt_rt<float>(x);
					case 0x3f80'0000: // y = 1.0f
						return x;
					case 0x4000'0000: // y = 2.0f
						return x * x;
					default:
						if (is_integer(y) && y_u > 0x4000'0000 && y_u <= 0x41c0'0000)
						{
							std::int_fast32_t msb			   = (x_abs == 0) ? (FloatBits::total_length - 2) : support::countl_zero(x_abs);
							msb								   = (msb > FloatBits::exponent_length) ? msb : FloatBits::exponent_length;
							std::int_fast32_t lsb			   = (x_abs == 0) ? 0 : support::countr_zero(x_abs);
							lsb								   = (lsb > FloatBits::fraction_length) ? FloatBits::fraction_length : lsb;
							const std::int_fast32_t extra_bits = FloatBits::total_length - 2 - lsb - msb;

							if (const auto iter = static_cast<std::int_fast32_t>(y); extra_bits * iter <= FloatBits::fraction_length + 2)
							{
								const auto x_d = static_cast<double>(x);
								double result  = x_d;
								for (int i = 1; i < iter; ++i) { result *= x_d; }
								return static_cast<float>(result);
							}
						}
						if (y_abs > 0x4f17'0000)
						{
							if (y_abs > 0x7f80'0000)
							{
								if (x_u == 0x3f80'0000) { return 1.0F; }
								return y;
							}
							y = support::bit_cast<float>((y_u & FloatBits::sign_mask) + 0x4f80'0000U);
						}
					}
				}
			}

			if (CCM_UNLIKELY(((x_u & 0x801f'ffffU) == 0) || x_u >= 0x7f80'0000U || x_u < 0x0080'0000U))
			{
				switch (x_u)
				{
				case 0x3f80'0000: // x = 1.0f
					return 1.0F;
				case 0x4000'0000: // x = 2.0f
								  // return generic::exp2f(y); // TODO: Implement
				case 0x4120'0000: // x = 10.0f
								  // return generic::exp10f(y); // TODO: Implement
				default: break;
				}

				const bool x_is_neg = x_u >= FloatBits::sign_mask;

				if (x == 0.0F)
				{
					const bool out_is_neg = x_is_neg && is_odd_integer(FloatBits(y_u).get_val());
					if (y_u > 0x8000'0000U) // pow(0, negative number) = inf
					{
						support::fenv::set_errno_if_required(EDOM);
						support::fenv::raise_except_if_required(FE_DIVBYZERO);
						return FloatBits::inf(out_is_neg ? types::Sign::NEG : types::Sign::POS).get_val();
					}
					return out_is_neg ? -0.0F : 0.0F; // pow(0, positive number) = 0
				}

				if (x_abs == 0x7f80'0000) // x is +/-inf
				{
					const bool out_is_neg = x_is_neg && is_odd_integer(FloatBits(y_u).get_val());
					if (y_u >= FloatBits::sign_mask) { return out_is_neg ? -0.0F : 0.0F; }
					return FloatBits::inf(out_is_neg ? types::Sign::NEG : types::Sign::POS).get_val();
				}

				if (x_abs > 0x7f80'0000)
				{
					return x; // x is NaN and pow(aNaN, 0) is already handled above
				}

				// Normalize denormal inputs
				if (x_abs < 0x0080'0000U)
				{
					exponent -= 64;
					x *= 0x1.0p64F;
				}

				// x is finite and negative, and y is a finite integer.
				if (x_is_neg)
				{
					if (is_integer(y))
					{
						x = -x;
						if (is_odd_integer(y)) { sign = 0x8000'0000'0000'0000ULL; } // sign = -1.0
					}
					else
					{
						support::fenv::set_errno_if_required(EDOM);
						support::fenv::raise_except_if_required(FE_INVALID);
						return FloatBits::quiet_nan().get_val();
					}
				}
			}

			return std::nullopt; // No exceptional case
		}

		constexpr float powf_impl(float x, float y)
		{
			using FloatBits	 = support::fp::FPBits<float>;
			using DoubleBits = support::fp::FPBits<double>;

			FloatBits xbits(x);
			FloatBits ybits(y);

			std::uint32_t x_u  = xbits.uintval();
			int ex			   = -FloatBits::exponent_bias;
			std::uint64_t sign = 0;

			// Check for exceptional cases
			if (auto exceptional_case = handle_exceptional_cases(x, y, xbits, ybits, x_u, ex, sign); CCM_UNLIKELY(exceptional_case.has_value()))
			{
				return *exceptional_case; // Return the handled value
			}

			// x^y = 2^( y * log2(x) )
			//     = 2^( y * ( e_x + log2(m_x) ) )
			// First we compute log2(x) = e_x + log2(m_x)
			x_u = FloatBits(x).uintval();

			// Extract exponent field of x.
			ex += (x_u >> FloatBits::fraction_length); // NOLINT(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
			const auto e_x = static_cast<double>(ex);
			// Use the highest 7 fractional bits of m_x as the index for look up tables.
			const std::uint32_t x_mant = x_u & FloatBits::fraction_mask;
			const int idx_x			   = static_cast<int>(x_mant >> (FloatBits::fraction_length - 7));
			// Add the hidden bit to the mantissa.
			// 1 <= m_x < 2
			const float m_x = support::bit_cast<float>(x_mant | 0x3f800000);

			// Reduced argument for log2(m_x):
			//   dx = r * m_x - 1.
			// The computation is exact, and -2^-8 <= dx < 2^-7.
			// Then m_x = (1 + dx) / r, and
			//   log2(m_x) = log2( (1 + dx) / r )
			//             = log2(1 + dx) - log2(r).
#ifdef CCMATH_TARGET_CPU_HAS_FMA
			const double dx = static_cast<double>(support::multiply_add(m_x, support::constants::R.at(idx_x), -1.0F)); // Exact
#else
			const double dx = support::multiply_add(static_cast<double>(m_x), support::constants::RD.at(idx_x), -1.0); // Exact
#endif // LIBC_TARGET_CPU_HAS_FMA

			// Degree-5 polynomial approximation:
			//   dx * P(dx) ~ log2(1 + dx)
			// Generated by Sollya with:
			// > P = fpminimax(log2(1 + x)/x, 5, [|D...|], [-2^-8, 2^-7]);
			// > dirtyinfnorm(log2(1 + x)/x - P, [-2^-8, 2^-7]);
			//   0x1.653...p-52
			constexpr std::array<double, 6> COEFFS = {0x1.71547652b82fep0,	 -0x1.71547652b7a07p-1, 0x1.ec709dc458db1p-2,
													  -0x1.715479c2266c9p-2, 0x1.2776ae1ddf8fp-2,	-0x1.e7b2178870157p-3};

			const double dx2 = dx * dx; // Exact
			const double c0	 = support::multiply_add(dx, COEFFS[1], COEFFS[0]);
			const double c1	 = support::multiply_add(dx, COEFFS[3], COEFFS[2]);
			const double c2	 = support::multiply_add(dx, COEFFS[5], COEFFS[4]);

			const double p = support::polyeval(dx2, c0, c1, c2);

			//////////////////////////////////////////////////////////////////////////////
			// NOTE: For some reason, this is significantly less efficient than above!
			//
			// > P = fpminimax(log2(1 + x)/x, 4, [|D...|], [-2^-8, 2^-7]);
			// > dirtyinfnorm(log2(1 + x)/x - P, [-2^-8, 2^-7]);
			//   0x1.d04...p-44
			// constexpr double COEFFS[] = {0x1.71547652b8133p0, -0x1.71547652d1e33p-1,
			//                              0x1.ec70a098473dep-2, -0x1.7154c5ccdf121p-2,
			//                              0x1.2514fd90a130ap-2};
			//
			// double dx2 = dx * dx;
			// double c0 = fputil::multiply_add(dx, COEFFS[1], COEFFS[0]);
			// double c1 = fputil::multiply_add(dx, COEFFS[3], COEFFS[2]);
			// double p = fputil::polyeval(dx2, c0, c1, COEFFS[4]);
			//////////////////////////////////////////////////////////////////////////////

			// s = e_x - log2(r) + dx * P(dx)
			// Approximation errors:
			//   |log2(x) - s| < ulp(e_x) + (bounds on dx) * (error bounds of P(dx))
			//                 = ulp(e_x) + 2^-7 * 2^-51
			//                 < 2^8 * 2^-52 + 2^-7 * 2^-43
			//                 ~ 2^-44 + 2^-50
			const double s = support::multiply_add(dx, p, support::constants::LOG2_R.at(idx_x) + e_x);

			// To compute 2^(y * log2(x)), we break the exponent into 3 parts:
			//   y * log(2) = hi + mid + lo, where
			//   hi is an integer
			//   mid * 2^6 is an integer
			//   |lo| <= 2^-7
			// Then:
			//   x^y = 2^(y * log2(x)) = 2^hi * 2^mid * 2^lo,
			// In which 2^mid is obtained from a look-up table of size 2^6 = 64 elements,
			// and 2^lo ~ 1 + lo * P(lo).
			// Thus, we have:
			//   hi + mid = 2^-6 * round( 2^6 * y * log2(x) )
			// If we restrict the output such that |hi| < 150, (hi + mid) uses (8 + 6)
			// bits, hence, if we use double precision to perform
			//   round( 2^6 * y * log2(x))
			// the lo part is bounded by 2^-7 + 2^(-(52 - 14)) = 2^-7 + 2^-38

			// In the following computations:
			//   y6  = 2^6 * y
			//   hm  = 2^6 * (hi + mid) = round(2^6 * y * log2(x)) ~ round(y6 * s)
			//   lo6 = 2^6 * lo = 2^6 * (y - (hi + mid)) = y6 * log2(x) - hm.
			const auto y6	= static_cast<double>(y * 0x1.0p6F); // Exact.
			const double hm = support::fp::nearest_integer(s * y6);
			// lo6 = 2^6 * lo.
			const double lo6_hi = support::multiply_add(y6, e_x + LOG2_R_TD.at(idx_x).hi, -hm); // Exact
			// Error bounds:
			//   | (y*log2(x) - hm * 2^-6 - lo) / y| < err(dx * p) + err(LOG2_R_DD.lo)
			//                                       < 2^-51 + 2^-75
			const double lo6 = support::multiply_add(y6, support::multiply_add(dx, p, LOG2_R_TD.at(idx_x).mid), lo6_hi);

			// |2^(hi + mid) - exp2_hi_mid| <= ulp(exp2_hi_mid) / 2
			// Clamp the exponent part into a smaller range that fits double precision.
			// For those exponents that are out of range, the final conversion will round
			// them correctly to inf/max float or 0/min float accordingly.
			auto hm_i = static_cast<std::int64_t>(hm);
			hm_i	  = (hm_i > (1 << 15)) ? (1 << 15) : (hm_i < (-(1 << 15)) ? -(1 << 15) : hm_i);

			int idx_y = hm_i & 0x3f;

			// 2^hi
			std::int64_t exp_hi_i = (hm_i >> 6) << DoubleBits::fraction_length;

			// 2^mid
			// NOLINTNEXTLINE(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
			const std::int64_t exp_mid_i = support::bit_cast<std::uint64_t>(support::constants::EXP2_MID1.at(idx_y).hi);

			// (-1)^sign * 2^hi * 2^mid
			// Error <= 2^hi * 2^-53
			const std::uint64_t exp2_hi_mid_i = static_cast<std::uint64_t>(exp_hi_i + exp_mid_i) + sign;
			const double exp2_hi_mid		  = support::bit_cast<double>(exp2_hi_mid_i);

			// Degree-5 polynomial approximation P(lo6) ~ 2^(lo6 / 2^6) = 2^(lo).
			// Generated by Sollya with:
			// > P = fpminimax(2^(x/64), 5, [|1, D...|], [-2^-1, 2^-1]);
			// > dirtyinfnorm(2^(x/64) - P, [-0.5, 0.5]);
			// 0x1.a2b77e618f5c4c176fd11b7659016cde5de83cb72p-60
			constexpr std::array<double, 6> EXP2_COEFFS = {
				0x1p0, 0x1.62e42fefa39efp-7, 0x1.ebfbdff82a23ap-15, 0x1.c6b08d7076268p-23, 0x1.3b2ad33f8b48bp-31, 0x1.5d870c4d84445p-40};

			const double lo6_sqr = lo6 * lo6;
			const double d0		 = support::multiply_add(lo6, EXP2_COEFFS[1], EXP2_COEFFS[0]);
			const double d1		 = support::multiply_add(lo6, EXP2_COEFFS[3], EXP2_COEFFS[2]);
			const double d2		 = support::multiply_add(lo6, EXP2_COEFFS[5], EXP2_COEFFS[4]);
			const double pp		 = support::polyeval(lo6_sqr, d0, d1, d2);

			const double r = pp * exp2_hi_mid;

			// Ziv accuracy test
			const std::uint64_t r_u = support::bit_cast<uint64_t>(r);
			const float r_upper		= static_cast<float>(support::bit_cast<double>(r_u + error_tolerance));

			if (const float r_lower = static_cast<float>(support::bit_cast<double>(r_u - error_tolerance)); CCM_LIKELY(r_upper == r_lower))
			{
				// Check for overflow or underflow
				if (CCM_UNLIKELY(FloatBits(r_upper).get_mantissa() == 0))
				{
					if (FloatBits(r_upper).is_inf())
					{
						support::fenv::set_errno_if_required(ERANGE);
						support::fenv::raise_except_if_required(FE_OVERFLOW);
					}
					else if (r_upper == 0.0F)
					{
						support::fenv::set_errno_if_required(ERANGE);
						support::fenv::raise_except_if_required(FE_UNDERFLOW);
					}
				}
				return r_upper;
			}

			// Scale lower part of 2^(hi + mid)
			DoubleDouble exp2_hi_mid_dd{};
			exp2_hi_mid_dd.lo =
				(idx_y != 0) ? support::bit_cast<double>(exp_hi_i + support::bit_cast<int64_t>(support::constants::EXP2_MID1.at(idx_y).mid)) : 0.0;
			exp2_hi_mid_dd.hi = exp2_hi_mid;

			const double r_dd = powf_double_double(idx_x, dx, y6, lo6_hi, exp2_hi_mid_dd);

			return static_cast<float>(r_dd);
		}
	} // namespace internal::impl

	constexpr float powf_impl(float base, float exp) noexcept
	{
		return internal::impl::powf_impl(base, exp);
		// return internal::impl::powf_impl(base, exp);
	}

} // namespace ccm::gen::impl
