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
#include "ccmath/internal/types/double_double.hpp"
#include "ccmath/math/trig/impl/sincos_double_data.hpp"

#include <cerrno>
#include <cfenv>
#include <cstdint>

namespace ccm::internal::impl
{
	namespace sincos_double_detail
	{
		using FPBits		 = support::fp::FPBits<double>;
		using DoubleDouble	 = types::DoubleDouble;
		namespace data		 = sincos_double_data;

		constexpr unsigned sincos_range_reduction_small(double x, double & u)
		{
			const double prod_hi = x * data::ONE_OVER_PI;
			const double k		 = support::fp::nearest_integer(prod_hi);

			const double y_hi = support::multiply_add(k, data::MPI[0], x);
			u				  = support::multiply_add(k, data::MPI[1], y_hi);
			u				  = support::multiply_add(k, data::MPI[2], u);
			return static_cast<unsigned>(static_cast<int>(k));
		}

		constexpr unsigned sincos_range_reduction_large(double x, double & u)
		{
			FPBits xbits(x);

			const int x_e_m32 = xbits.get_biased_exponent() - (FPBits::exponent_bias + 32);
			unsigned idx	  = static_cast<unsigned>((x_e_m32 >> 3) + 2);
			if (idx >= data::EIGHT_OVER_PI.size()) { idx = static_cast<unsigned>(data::EIGHT_OVER_PI.size() - 1); }

			xbits.set_biased_exponent((x_e_m32 & 7) + FPBits::exponent_bias + 32);
			const double x_reduced = xbits.get_val();

			const DoubleDouble ph = types::exact_mult(x_reduced, data::EIGHT_OVER_PI[idx][0]);
			const DoubleDouble pm = types::exact_mult(x_reduced, data::EIGHT_OVER_PI[idx][1]);
			const DoubleDouble pl = types::exact_mult(x_reduced, data::EIGHT_OVER_PI[idx][2]);

			const double sum_hi = ph.lo + pm.hi;
			const double k		= support::fp::nearest_integer(sum_hi);

			const double y_hi		 = (ph.lo - k) + pm.hi;
			const DoubleDouble y_mid = types::exact_add(pm.lo, pl.hi);
			const double y_lo		 = pl.lo;

			const double y_l = support::multiply_add(x_reduced, data::EIGHT_OVER_PI[idx][3], y_lo);
			const DoubleDouble y = types::exact_add(y_hi, y_mid.hi);
			DoubleDouble y_total = y;
			y_total.lo += (y_mid.lo + y_l);

			u = support::multiply_add(y_total.hi, data::PI_OVER_8.hi, y_total.lo * data::PI_OVER_8.hi);

			return static_cast<unsigned>(static_cast<int>(k));
		}

		template <bool IsSin>
		constexpr double sincos_eval(double x)
		{
			FPBits xbits(x);
			const std::uint64_t x_abs = ccm::support::bit_cast<std::uint64_t>(x) & 0x7fff'ffff'ffff'ffffULL;

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
					}
					return x + FPBits::quiet_nan().get_val();
				}

				k = sincos_range_reduction_large(x, y);
			}

			const double sin_k = data::SIN_K_PI_OVER_8[k & 15];
			const double cos_k = data::SIN_K_PI_OVER_8[(k + 4) & 15];

			const double y_sq = y * y;
			const double p1	  = support::multiply_add(y_sq, 0x1.111111111111p-7, -0x1.555555555555p-3);
			const double q1	  = support::multiply_add(y_sq, 0x1.54b8b8b8b8b8p-5, -0x1.fffffffffffp-2);
			const double y3	  = y_sq * y;
			const double c1	  = support::multiply_add(y_sq, q1, 1.0);
			const double s1	  = support::multiply_add(y3, p1, y);

			if constexpr (IsSin) { return support::multiply_add(cos_k, s1, sin_k * c1); }
			return support::multiply_add(cos_k, c1, -sin_k * s1);
		}

	} // namespace sincos_double_detail

	constexpr double sin_double_impl(double x)
	{
		return sincos_double_detail::sincos_eval<true>(x);
	}

	constexpr double cos_double_impl(double x)
	{
		return sincos_double_detail::sincos_eval<false>(x);
	}

} // namespace ccm::internal::impl

namespace ccm::internal
{
	constexpr double sin_double(double num) noexcept
	{
		return impl::sin_double_impl(num);
	}

	constexpr double cos_double(double num) noexcept
	{
		return impl::cos_double_impl(num);
	}
} // namespace ccm::internal
