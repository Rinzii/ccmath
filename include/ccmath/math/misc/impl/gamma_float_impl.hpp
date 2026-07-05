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
#include "ccmath/math/misc/impl/gamma_double_impl.hpp"
#include "ccmath/math/nearest/floor.hpp"

namespace ccm::internal::impl
{
	constexpr float gamma_float_impl(float x) noexcept
	{
		using fp_bits = ccm::support::fp::FPBits<float>;
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

		if (CCM_UNLIKELY(!xbits.is_neg() && x >= 34.8F))
		{
			ccm::support::fenv::raise_except_if_required(FE_OVERFLOW | FE_INEXACT);
			ccm::support::fenv::set_errno_if_required(ERANGE);
			return fp_bits::inf().get_val();
		}

		const double result = gamma_double_impl(static_cast<double>(x));

		if (CCM_UNLIKELY(!xbits.is_neg() && ccm::support::fp::FPBits<double>(result).is_inf()))
		{
			return fp_bits::max_normal().get_val();
		}

		if (CCM_UNLIKELY(xbits.is_neg() && result != 0.0 && fp_bits(static_cast<float>(result)).is_zero()))
		{
			ccm::support::fenv::set_errno_if_required(ERANGE);
			const bool odd = static_cast<std::int32_t>(ccm::floor(static_cast<double>(x))) & 1;
			return odd ? -0.0F : 0.0F;
		}

		return static_cast<float>(result);
	}
} // namespace ccm::internal::impl
