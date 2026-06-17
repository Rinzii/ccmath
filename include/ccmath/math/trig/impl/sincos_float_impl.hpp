/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// Float sin/cos adapted from LLVM-libc (libc/src/__support/math/sincosf_float_eval.h).

#pragma once

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/support/fp/nearest_integer.hpp"
#include "ccmath/internal/support/multiply_add.hpp"
#include "ccmath/math/trig/impl/sincos_float_data.hpp"
#include "ccmath/math/trig/impl/sincos_payne_hanek.hpp"

#include <cerrno>
#include <cfenv>
#include <cstdint>

namespace ccm::internal::impl
{
	namespace sincos_float_detail
	{
		using FPBits   = support::fp::FPBits<float>;
		namespace data = sincos_float_data;

		constexpr unsigned sincosf_range_reduction_small(float x, float & u)
		{
			const float prod_hi = x * data::EIGHT_OVER_PI;
			const float k		= support::fp::nearest_integer(prod_hi);

			const float y_hi = support::multiply_add(k, data::MPI[0], x);
			u				 = support::multiply_add(k, data::MPI[1], y_hi);
			u				 = support::multiply_add(k, data::MPI[2], u);
			return static_cast<unsigned>(static_cast<int>(k));
		}

		template <bool IsSin>
		constexpr float sincosf_eval(float x)
		{
			FPBits xbits(x);
			const std::uint32_t x_abs = ccm::support::bit_cast<std::uint32_t>(x) & 0x7fff'ffffU;

			// sin(+/-0) = +/-0, cos(+/-0) = 1. Returning x preserves the sign of a signed zero.
			if (x_abs == 0) { return IsSin ? x : 1.0f; }

			float y{};
			unsigned k = 0;

			if (x_abs < 0x4880'0000U) { k = sincosf_range_reduction_small(x, y); }
			else
			{
				if (CCM_UNLIKELY(x_abs >= 0x7f80'0000U))
				{
					if (xbits.is_signaling_nan())
					{
						support::fenv::raise_except_if_required(FE_INVALID);
						return FPBits::quiet_nan().get_val();
					}

					if (x_abs == 0x7f80'0000U)
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

				double yd = 0.0;
				k		  = sincos_ph::payne_hanek_reduce(static_cast<double>(x), yd);
				y		  = static_cast<float>(yd);
			}

			const float sin_k = data::SIN_K_PI_OVER_8[k & 15];
			const float cos_k = data::SIN_K_PI_OVER_8[(k + 4) & 15];

			// sin(y) = y + y^3 p1(y^2) and cos(y) = 1 + y^2 q1(y^2) on |y| < pi/16, with float-grade
			// two-term tails (the double kernel uses higher-order polynomials).
			const float y_sq = y * y;
			const float p1	 = support::multiply_add(y_sq, 0x1.111112p-7f, -0x1.555556p-3f);
			const float q1	 = support::multiply_add(y_sq, 0x1.54b8bep-5f, -0x1.ffffc4p-2f);
			const float y3	 = y_sq * y;
			const float c1	 = support::multiply_add(y_sq, q1, 1.0f);
			const float s1	 = support::multiply_add(y3, p1, y);

			// The cos tail is in the else so it is discarded for the sin instantiation rather than
			// left as unreachable code, which MSVC rejects under /W4 (C4702).
			if constexpr (IsSin) { return support::multiply_add(cos_k, s1, sin_k * c1); }
			else
			{
				return support::multiply_add(cos_k, c1, -sin_k * s1);
			}
		}

	} // namespace sincos_float_detail

	constexpr float sin_float_impl(float x)
	{ return sincos_float_detail::sincosf_eval<true>(x); }

	constexpr float cos_float_impl(float x)
	{ return sincos_float_detail::sincosf_eval<false>(x); }

} // namespace ccm::internal::impl

namespace ccm::internal
{
	constexpr float sin_float(float num) noexcept
	{ return impl::sin_float_impl(num); }

	constexpr float cos_float(float num) noexcept
	{ return impl::cos_float_impl(num); }
} // namespace ccm::internal
