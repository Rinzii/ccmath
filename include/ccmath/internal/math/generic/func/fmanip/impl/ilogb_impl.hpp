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

#include "ccmath/internal/math/generic/func/fmanip/impl/frexp_impl.hpp"
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"

#include <cerrno>
#include <cfenv>
#include <climits>
#include <cstdint>

namespace ccm::internal::impl
{
	constexpr int fp_ilogb0() noexcept
	{
#ifdef FP_ILOGB0
		return FP_ILOGB0;
#else
		return INT_MIN;
#endif
	}

	constexpr int fp_ilogbnan() noexcept
	{
#ifdef FP_ILOGBNAN
		return FP_ILOGBNAN;
#else
		return INT_MIN;
#endif
	}

	template <typename T>
	constexpr int ilogb_impl(T x) noexcept
	{
		using fp_bits_t = ccm::support::fp::FPBits<T>;

		fp_bits_t const bits(x);

		if (CCM_UNLIKELY(bits.is_nan()))
		{
			ccm::support::fenv::set_errno_if_required(EDOM);
			ccm::support::fenv::raise_except_if_required(FE_INVALID);
			return fp_ilogbnan();
		}

		if (CCM_UNLIKELY(bits.is_inf()))
		{
			ccm::support::fenv::set_errno_if_required(EDOM);
			ccm::support::fenv::raise_except_if_required(FE_INVALID);
			return INT_MAX;
		}

		if (CCM_UNLIKELY(bits.is_zero()))
		{
			ccm::support::fenv::set_errno_if_required(EDOM);
			ccm::support::fenv::raise_except_if_required(FE_INVALID);
			return fp_ilogb0();
		}

		int exp{};
		frexp_impl(x, exp);
		return exp - 1;
	}
} // namespace ccm::internal::impl
