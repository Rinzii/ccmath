/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// Portions of this code were borrowed from the LLVM Project,
// under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fenv/host_fenv.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/support/fp/nearest_integer.hpp"
#include "ccmath/internal/support/multiply_add.hpp"
#include "ccmath/math/misc/impl/gamma_data.hpp"
#include "ccmath/math/nearest/floor.hpp"

#include <cstdint>

namespace ccm::internal::impl
{
	namespace detail
	{
		using fp_bits = ccm::support::fp::FPBits<double>;

		constexpr bool is_integer(double x) noexcept
		{
			const std::uint64_t x_u				 = ccm::support::bit_cast<std::uint64_t>(x);
			const auto x_e						 = static_cast<std::int32_t>((x_u & fp_bits::exponent_mask) >> fp_bits::fraction_length);
			const std::int32_t lsb				 = ccm::support::countr_zero(x_u | fp_bits::exponent_mask);
			constexpr std::int32_t unit_exponent = fp_bits::exponent_bias + static_cast<std::int32_t>(fp_bits::fraction_length);
			return x_e + lsb >= unit_exponent;
		}

		constexpr double gamma_polynomial(double d) noexcept
		{
			const double d2	 = d * d;
			const double d4	 = d2 * d2;
			const double p01 = ccm::support::multiply_add(d, data::k_gamma_coeffs[1], data::k_gamma_coeffs[0]);
			const double p23 = ccm::support::multiply_add(d, data::k_gamma_coeffs[3], data::k_gamma_coeffs[2]);
			const double p45 = ccm::support::multiply_add(d, data::k_gamma_coeffs[5], data::k_gamma_coeffs[4]);
			const double p67 = ccm::support::multiply_add(d, data::k_gamma_coeffs[7], data::k_gamma_coeffs[6]);
			const double p03 = ccm::support::multiply_add(d2, p23, p01);
			const double p47 = ccm::support::multiply_add(d2, p67, p45);
			return ccm::support::multiply_add(d4, p47, p03);
		}

		constexpr double gamma_reduce(double xd, double & w_out) noexcept
		{
			const double m	  = xd - data::k_gamma_base;
			const double i	  = ccm::support::fp::nearest_integer(m);
			const double step = fp_bits(i).is_neg() ? -1.0 : 1.0;
			const auto jm	  = static_cast<std::int32_t>(i < 0.0 ? -i : i);
			const double d	  = m - i;
			const double f	  = gamma_polynomial(d);

			double z = xd;
			double w = 1.0;
			if (jm != 0)
			{
				z -= 0.5 + step * 0.5;
				w = z;
				for (std::int32_t j = jm - 1; j != 0; --j)
				{
					z -= step;
					w *= z;
				}
			}
			if (i <= -0.5)
			{
				w = 1.0 / w;
			}

			w_out = w;
			return f;
		}

		constexpr double positive_factorial(std::int32_t k) noexcept
		{
			double result = 1.0;
			for (std::int32_t i = 2; i < k; ++i)
			{
				result *= static_cast<double>(i);
			}
			return result;
		}

	} // namespace detail

	constexpr double gamma_double_impl(double x) noexcept
	{
		using detail::fp_bits;
		fp_bits xbits(x);

		if (CCM_UNLIKELY(xbits.is_nan() || xbits.is_inf()))
		{
			if (xbits.is_inf() && xbits.is_neg())
			{
				ccm::support::fenv::raise_except_if_required(FE_INVALID);
				ccm::support::fenv::set_errno_if_required(EDOM);
				return fp_bits::quiet_nan().get_val();
			}
			if (xbits.is_signaling_nan())
			{
				ccm::support::fenv::raise_except_if_required(FE_INVALID);
				return fp_bits::quiet_nan().get_val();
			}
			return x;
		}

		if (CCM_UNLIKELY(xbits.is_zero()))
		{
			ccm::support::fenv::raise_except_if_required(FE_DIVBYZERO);
			ccm::support::fenv::set_errno_if_required(ERANGE);
			return fp_bits::inf(xbits.sign()).get_val();
		}

		if (CCM_UNLIKELY(x <= 0x1p-53 && x >= -0x1p-53))
		{
			const double d = (data::k_tiny_a - data::k_tiny_b * x) * x - data::k_tiny_c;
			const double f = 1.0 / x + d;
			if (CCM_UNLIKELY(fp_bits(f).is_inf()))
			{
				ccm::support::fenv::raise_except_if_required(FE_OVERFLOW | FE_INEXACT);
				ccm::support::fenv::set_errno_if_required(ERANGE);
			} else
			{
				ccm::support::fenv::raise_except_if_required(FE_INEXACT);
			}
			return f;
		}

		if (CCM_UNLIKELY(!xbits.is_neg() && x >= 171.6263688021478))
		{
			ccm::support::fenv::raise_except_if_required(FE_OVERFLOW | FE_INEXACT);
			ccm::support::fenv::set_errno_if_required(ERANGE);
			return fp_bits::inf().get_val();
		}

		if (CCM_UNLIKELY(detail::is_integer(x)))
		{
			const auto k = static_cast<std::int32_t>(ccm::floor(x));
			if (xbits.is_neg())
			{
				ccm::support::fenv::raise_except_if_required(FE_INVALID);
				ccm::support::fenv::set_errno_if_required(EDOM);
				return fp_bits::quiet_nan().get_val();
			}
			if (k >= 1 && k <= 170)
			{
				return detail::positive_factorial(k);
			}
		}

		double w{};
		const double f		= detail::gamma_reduce(x, w);
		const double result = f * w;
		if (CCM_UNLIKELY(fp_bits(result).is_inf()))
		{
			ccm::support::fenv::raise_except_if_required(FE_OVERFLOW | FE_INEXACT);
			ccm::support::fenv::set_errno_if_required(ERANGE);
		} else if (CCM_UNLIKELY(result == 0.0 && !xbits.is_neg()))
		{
			ccm::support::fenv::set_errno_if_required(ERANGE);
		}
		return result;
	}
} // namespace ccm::internal::impl
