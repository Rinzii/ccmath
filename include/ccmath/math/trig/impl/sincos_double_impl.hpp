/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// Double sin/cos adapted from sincos_float_impl.hpp (LLVM-libc float core).

#pragma once

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/support/fp/nearest_integer.hpp"
#include "ccmath/internal/support/multiply_add.hpp"
#include "ccmath/math/trig/impl/sincos_double_data.hpp"
#include "ccmath/math/trig/impl/sincos_payne_hanek.hpp"

#include <cerrno>
#include <cfenv>
#include <cstdint>

namespace ccm::internal::impl
{
	namespace sincos_double_detail
	{
		using FPBits   = support::fp::FPBits<double>;
		namespace data = sincos_double_data;

		constexpr unsigned sincos_range_reduction_small(double x, double & u)
		{
			const double prod_hi = x * data::EIGHT_OVER_PI;
			const double k		 = support::fp::nearest_integer(prod_hi);

			const double y_hi = support::multiply_add(k, data::MPI[0], x);
			u				  = support::multiply_add(k, data::MPI[1], y_hi);
			u				  = support::multiply_add(k, data::MPI[2], u);
			return static_cast<unsigned>(static_cast<int>(k));
		}

		template <bool IsSin>
		constexpr double sincos_eval(double x)
		{
			FPBits xbits(x);
			const std::uint64_t x_abs = ccm::support::bit_cast<std::uint64_t>(x) & 0x7fff'ffff'ffff'ffffULL;

			// sin(+/-0) = +/-0, cos(+/-0) = 1. Returning x preserves the sign of a signed zero,
			// which the reconstruction below would otherwise flush to +0.
			if (x_abs == 0) { return IsSin ? x : 1.0; }

			double y{};
			unsigned k = 0;

			if (x_abs < 0x4130'0000'0000'0000ULL) { k = sincos_range_reduction_small(x, y); }
			else
			{
				if (CCM_UNLIKELY(x_abs >= 0x7ff0'0000'0000'0000ULL))
				{
					if (xbits.is_signaling_nan())
					{
						support::fenv::raise_except_if_required(FE_INVALID);
						return FPBits::quiet_nan().get_val();
					}

					if (x_abs == 0x7ff0'0000'0000'0000ULL)
					{
						support::fenv::set_errno_if_required(EDOM);
						support::fenv::raise_except_if_required(FE_INVALID);
						// sin/cos of an infinity is a domain error, so return a canonical quiet NaN.
						// Forming it as x + quiet_nan is ill-formed in a constant expression, which
						// rejects floating-point arithmetic that produces a NaN.
						return FPBits::quiet_nan().get_val();
					}

					// x is a quiet NaN here. Return it unchanged so its sign and payload survive.
					return x;
				}

				k = sincos_ph::payne_hanek_reduce(x, y);
			}

			const double sin_k = data::SIN_K_PI_OVER_8[k & 15];
			const double cos_k = data::SIN_K_PI_OVER_8[(k + 4) & 15];

			const double y_sq = y * y;

			// sin(y) = y * (1 + y^2 (SIN_POLY[1] + y^2 (...))) and cos(y) = 1 + y^2 (COS_POLY[1] + ...),
			// evaluated by Horner. The double-precision coefficients keep |y| < pi/16 to ~2^-63
			// (sin) and ~2^-68 (cos), versus the float-grade two-term tails this replaced.
			double sp = support::multiply_add(y_sq, data::SIN_POLY[5], data::SIN_POLY[4]);
			sp		  = support::multiply_add(y_sq, sp, data::SIN_POLY[3]);
			sp		  = support::multiply_add(y_sq, sp, data::SIN_POLY[2]);
			sp		  = support::multiply_add(y_sq, sp, data::SIN_POLY[1]);

			double cp = support::multiply_add(y_sq, data::COS_POLY[5], data::COS_POLY[4]);
			cp		  = support::multiply_add(y_sq, cp, data::COS_POLY[3]);
			cp		  = support::multiply_add(y_sq, cp, data::COS_POLY[2]);
			cp		  = support::multiply_add(y_sq, cp, data::COS_POLY[1]);

			const double y3 = y_sq * y;
			const double c1 = support::multiply_add(y_sq, cp, 1.0);
			const double s1 = support::multiply_add(y3, sp, y);

			// The cos tail is in the else so it is discarded for the sin instantiation rather than
			// left as unreachable code, which MSVC rejects under /W4 (C4702).
			if constexpr (IsSin) { return support::multiply_add(cos_k, s1, sin_k * c1); }
			else
			{
				return support::multiply_add(cos_k, c1, -sin_k * s1);
			}
		}

	} // namespace sincos_double_detail

	constexpr double sin_double_impl(double x)
	{ return sincos_double_detail::sincos_eval<true>(x); }

	constexpr double cos_double_impl(double x)
	{ return sincos_double_detail::sincos_eval<false>(x); }

} // namespace ccm::internal::impl

namespace ccm::internal
{
	constexpr double sin_double(double num) noexcept
	{ return impl::sin_double_impl(num); }

	constexpr double cos_double(double num) noexcept
	{ return impl::cos_double_impl(num); }
} // namespace ccm::internal
