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
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fenv/host_fenv.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/math/basic/fabs.hpp"
#include "ccmath/math/expo/log.hpp"
#include "ccmath/math/misc/impl/gamma_data.hpp"
#include "ccmath/math/misc/impl/gamma_double_impl.hpp"
#include "ccmath/math/nearest/floor.hpp"
#include "ccmath/math/numbers.hpp"
#include "ccmath/math/trig/sin.hpp"

namespace ccm::internal::impl
{
	namespace lgamma_detail
	{
		using fp_bits = ccm::support::fp::FPBits<double>;

		constexpr double log_abs(double x) noexcept
		{ return ccm::log(ccm::fabs(x)); }

		constexpr double stirling(double x) noexcept
		{
			const double inv_x	= 1.0 / x;
			const double inv_x2 = inv_x * inv_x;
			const double corr	= inv_x * (0x1.5555555555555p-3 + inv_x2 * (-0x1.6c16c16c16c17p-7 + inv_x2 * 0x1.00a01a01a01a1p-7));
			return (x - 0.5) * ccm::log(x) - x + 0.5 * ccm::log(2.0 * ccm::numbers::pi_v<double>) + corr;
		}

		constexpr double log_gamma_reduce(double xd) noexcept
		{
			double w{};
			const double f = detail::gamma_reduce(xd, w);
			return ccm::log(f) + log_abs(w);
		}

		constexpr double positive_lfactorial(std::int32_t k) noexcept
		{
			if (k <= 2) { return 0.0; }
			double sum = 0.0;
			for (std::int32_t i = 2; i < k; ++i) { sum += ccm::log(static_cast<double>(i)); }
			return sum;
		}

	} // namespace lgamma_detail

	constexpr double lgamma_double_impl(double x) noexcept
	{
		using lgamma_detail::fp_bits;
		fp_bits xbits(x);

		if (CCM_UNLIKELY(xbits.is_nan()))
		{
			if (xbits.is_signaling_nan())
			{
				ccm::support::fenv::raise_except_if_required(FE_INVALID);
				return fp_bits::quiet_nan().get_val();
			}
			return x;
		}

		if (CCM_UNLIKELY(xbits.is_inf()))
		{
			if (xbits.is_neg())
			{
				ccm::support::fenv::raise_except_if_required(FE_INVALID);
				ccm::support::fenv::set_errno_if_required(EDOM);
			}
			return fp_bits::inf().get_val();
		}

		if (CCM_UNLIKELY(xbits.is_zero()))
		{
			ccm::support::fenv::raise_except_if_required(FE_DIVBYZERO);
			ccm::support::fenv::set_errno_if_required(ERANGE);
			return fp_bits::inf().get_val();
		}

		if (CCM_UNLIKELY(detail::is_integer(x)))
		{
			if (x <= 0.0)
			{
				ccm::support::fenv::raise_except_if_required(FE_INVALID);
				ccm::support::fenv::set_errno_if_required(EDOM);
				return fp_bits::inf().get_val();
			}
			const auto k = static_cast<std::int32_t>(ccm::floor(x));
			if (k <= 170) { return lgamma_detail::positive_lfactorial(k); }
		}

		if (CCM_UNLIKELY(ccm::fabs(x) <= 0x1p-53)) { return -lgamma_detail::log_abs(x); }

		if (CCM_UNLIKELY(x >= 8.0)) { return lgamma_detail::stirling(x); }

		if (CCM_UNLIKELY(x < 0.0))
		{
			return ccm::log(ccm::numbers::pi_v<double>) - lgamma_detail::log_abs(x) - lgamma_detail::log_abs(ccm::sin(ccm::numbers::pi_v<double> * x)) -
				   lgamma_double_impl(-x);
		}

		return lgamma_detail::log_gamma_reduce(x);
	}

	constexpr float lgamma_float_impl(float x) noexcept
	{ return static_cast<float>(lgamma_double_impl(static_cast<double>(x))); }
} // namespace ccm::internal::impl
