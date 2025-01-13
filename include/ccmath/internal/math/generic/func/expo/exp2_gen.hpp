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

#include "ccmath/internal/support/floating_point_traits.hpp"
#include "ccmath/internal/support/fp/bit_mask_traits.hpp"
#include "ccmath/internal/support/fp/directional_rounding_utils.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/support/unreachable.hpp"
#include "ccmath/internal/types/big_int.hpp"
#include "ccmath/math/basic/fabs.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/math/power/sqrt.hpp"

#include <limits>

#include "ccmath/internal/support/poly_eval.hpp"
#include "ccmath/internal/types/double_double.hpp"

namespace ccm::gen
{
	namespace internal::impl
	{
		using namespace types;
		using Float128 = DyadicFloat<128>;

		using ccm::support::operator""_u128;

#ifdef CCMATH_TARGET_CPU_HAS_FMA
		constexpr double error_bounds_d = 0x1.0p-63;
#else
		constexpr double error_bounds_d = 0x1.8p-63;
#endif
		constexpr double error_bounds_dd = 0x1.0p-100;

		/**
		 * @brief Approximates the expression (2^x - 1)/x using a degree-3 polynomial.
		 *
		 * This function computes an approximation for (2^x - 1)/x with high accuracy
		 * over the range [-2^-13 - 2^-30, 2^-13 + 2^-30].
		 * The coefficients were generated using Sollya to minimize error.
		 * The maximum error for this approximation is less than 1.5 * 2^-52.
		 *
		 * Polynomial generation command in Sollya:
		 * @code
		 * P = fpminimax((2^x - 1)/x, 3, [|D...|], [-2^-13 - 2^-30, 2^-13 + 2^-30]);
		 * @endcode
		 *
		 * @param dx Input value for which the approximation is computed.
		 * @return The approximate value of (2^x - 1)/x.
		 */
		inline double poly_approx_d(double dx)
		{
			// dx^2
			const double dx2 = dx * dx;
			const double c0	 = support::multiply_add(dx, 0x1.ebfbdff82c58ep-3, 0x1.62e42fefa39efp-1);
			const double c1	 = support::multiply_add(dx, 0x1.3b2aba7a95a89p-7, 0x1.c6b08e8fc0c0ep-5);
			const double p	 = support::multiply_add(dx2, c1, c0);
			return p;
		}

		/**
		 * @brief Approximates the expression (2^x - 1)/x using a degree-5 polynomial in double-double precision.
		 *
		 * This function computes a high-precision approximation for `(2^x - 1)/x` using double-double arithmetic.
		 * The coefficients were generated with Sollya to minimize error over the range `[-2^-13 - 2^-30, 2^-13 + 2^-30]`.
		 * The approximation has a maximum error of less than `2^-101` when compared to the actual value of `2^x`.
		 *
		 * Polynomial generation in Sollya:
		 * @code
		 * P = fpminimax((2^x - 1)/x, 5, [|DD...|], [-2^-13 - 2^-30, 2^-13 + 2^-30]);
		 * @endcode
		 *
		 * @param dx The input value as a double-double for which the approximation is calculated.
		 * @return A double-double representation of the approximation of `(2^x - 1)/x`.
		 */
		inline DoubleDouble poly_approx_dd(const DoubleDouble & dx)
		{
			// Taylor polynomial coefficients
			constexpr std::array<DoubleDouble, 7> COEFFS = {
				DoubleDouble{0, 0x1p0},
				DoubleDouble{0x1.abc9e3b39824p-56, 0x1.62e42fefa39efp-1},
				DoubleDouble{-0x1.5e43a53e4527bp-57, 0x1.ebfbdff82c58fp-3},
				DoubleDouble{-0x1.d37963a9444eep-59, 0x1.c6b08d704a0cp-5},
				DoubleDouble{0x1.4eda1a81133dap-62, 0x1.3b2ab6fba4e77p-7},
				DoubleDouble{-0x1.c53fd1ba85d14p-64, 0x1.5d87fe7a265a5p-10},
				DoubleDouble{0x1.d89250b013eb8p-70, 0x1.430912f86cb8ep-13},
			};

			const DoubleDouble p = support::polyeval(dx, COEFFS[0], COEFFS[1], COEFFS[2], COEFFS[3], COEFFS[4], COEFFS[5], COEFFS[6]);
			return p;
		}

		// Polynomial approximation with 128-bit precision:
		// Return exp(dx) ~ 1 + a0 * dx + a1 * dx^2 + ... + a6 * dx^7
		// For |dx| < 2^-13 + 2^-30:
		//   | output - exp(dx) | < 2^-126.
		inline Float128 poly_approx_f128(const Float128 & dx)
		{
			constexpr std::array<Float128, 8> COEFFS_128{
				Float128{Sign::POS, -127, 0x80000000'00000000'00000000'00000000_u128}, // 1.0
				Float128{Sign::POS, -128, 0xb17217f7'd1cf79ab'c9e3b398'03f2f6af_u128}, Float128{Sign::POS, -128, 0x3d7f7bff'058b1d50'de2d60dd'9c9a1d9f_u128},
				Float128{Sign::POS, -132, 0xe35846b8'2505fc59'9d3b15d9'e7fb6897_u128}, Float128{Sign::POS, -134, 0x9d955b7d'd273b94e'184462f6'bcd2b9e7_u128},
				Float128{Sign::POS, -137, 0xaec3ff3c'53398883'39ea1bb9'64c51a89_u128}, Float128{Sign::POS, -138, 0x2861225f'345c396a'842c5341'8fa8ae61_u128},
				Float128{Sign::POS, -144, 0xffe5fe2d'109a319d'7abeb5ab'd5ad2079_u128},
			};

			const Float128 p =
				support::polyeval(dx, COEFFS_128[0], COEFFS_128[1], COEFFS_128[2], COEFFS_128[3], COEFFS_128[4], COEFFS_128[5], COEFFS_128[6], COEFFS_128[7]);
			return p;
		}

		template <typename T>
		constexpr std::enable_if_t<std::is_floating_point_v<T>, T> exp2_impl(T x, T y) noexcept
		{

			return 0;
		}
	} // namespace internal::impl

	template <typename T>
	constexpr std::enable_if_t<std::is_floating_point_v<T>, T> exp2_gen(T base, T exp) noexcept
	{
		return internal::impl::exp2_impl(base, exp);
	}
} // namespace ccm::gen
