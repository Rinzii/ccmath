/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/multiply_add.hpp"
#include "ccmath/internal/types/double_double.hpp"
#include "ccmath/internal/types/triple_double.hpp"
#include "ccmath/math/exponential/log2.hpp"

#include <array>

#include "ccmath/internal/support/poly_eval.hpp"

namespace ccm::internal
{
	namespace impl
	{
		using namespace ccm::type;

		constexpr double LOG2_10 = ccm::log2(10.0);

		// -2^-12 * log10(2)
		// > a = -2^-12 * log10(2);
		// > b = round(a, 32, RN);
		// > c = round(a - b, 32, RN);
		// > d = round(a - b - c, D, RN);
		// Errors < 1.5 * 2^-144
		constexpr double MLOG10_2_EXP2_M12_HI	  = -0x1.3441350ap-14;
		constexpr double MLOG10_2_EXP2_M12_MID	  = 0x1.0c0219dc1da99p-51;
		constexpr double MLOG10_2_EXP2_M12_MID_32 = 0x1.0c0219dcp-51;
		constexpr double MLOG10_2_EXP2_M12_LO	  = 0x1.da994fd20dba2p-87;

		// Error bounds for double precision
		constexpr double ERROR_D = 0x1.8p-63;

		// Error bounds for double-double precision
		constexpr double ERROR_DD = 0x1.8p-99;

		// Polynomail approximation for double precision.
		// Generated values with Sollya using the following script:
		// > P = fpminimax((10^x - 1)/x, 3, [|D...|], [-2^-14, 2^-14]);
		// > P;
		// Error bounds:
		//		| output - (10^dx - 1) / dx | < 2^-52.
		constexpr double polynomial_approx_d(double x)
		{
			const double x_sqr = x * x;
			const double c0	   = support::multiply_add(x, 0x1.53524c73cea6ap+1, 0x1.26bb1bbb55516p+1);
			const double c1	   = support::multiply_add(x, 0x1.2bd75cc6afc65p+0, 0x1.0470587aa264cp+1);
			const double p	   = support::multiply_add(x_sqr, c1, c0);
			return p;
		}

		constexpr DoubleDouble polynomial_approx_dd(DoubleDouble x)
		{
			// Taylor polynomial.
			constexpr std::array<DoubleDouble, 7> COEFFS = {
				DoubleDouble{0, 0x1p0},
				DoubleDouble{-0x1.f48ad494e927bp-53, 0x1.26bb1bbb55516p1},
				DoubleDouble{-0x1.e2bfab3191cd2p-53, 0x1.53524c73cea69p1},
				DoubleDouble{0x1.80fb65ec3b503p-53, 0x1.0470591de2ca4p1},
				DoubleDouble{0x1.338fc05e21e55p-54, 0x1.2bd7609fd98c4p0},
				DoubleDouble{0x1.d4ea116818fbp-56, 0x1.1429ffd519865p-1},
				DoubleDouble{-0x1.872a8ff352077p-57, 0x1.a7ed70847c8b3p-3},
			};

			const DoubleDouble p = support::polyeval(x, COEFFS[0], COEFFS[1], COEFFS[2], COEFFS[3], COEFFS[4], COEFFS[5], COEFFS[6]);
			return p;
		}

		constexpr double exp10_double_impl(double exp) noexcept
		{

			return 0;
		}

		constexpr double exp10_double_double_impl(double exp) noexcept
		{

			return 0;
		}

		constexpr float exp10_float_impl(float exp) noexcept
		{

			return 0;
		}
	} // namespace impl

	constexpr double exp10_double(double exp) noexcept
	{
		return impl::exp10_double_impl(exp);
	}

	constexpr double exp10_double_double(double exp) noexcept
	{
		return impl::exp10_double_double_impl(exp);
	}

	constexpr float exp10_float(float exp) noexcept
	{
		return impl::exp10_float_impl(exp);
	}
} // namespace ccm::internal