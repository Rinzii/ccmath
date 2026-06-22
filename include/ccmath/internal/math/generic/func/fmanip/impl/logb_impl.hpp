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

#include "ccmath/internal/math/generic/func/fmanip/impl/ilogb_impl.hpp"
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fenv/host_fenv.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"

namespace ccm::internal::impl
{
	template <typename T>
	constexpr T logb_impl(T x) noexcept
	{
		using fp_bits_t = ccm::support::fp::FPBits<T>;

		fp_bits_t const bits(x);

		if (CCM_UNLIKELY(bits.is_nan())) { return x; }

		if (CCM_UNLIKELY(bits.is_inf())) { return fp_bits_t::inf().get_val(); }

		if (CCM_UNLIKELY(bits.is_zero()))
		{
			ccm::support::fenv::raise_except_if_required(FE_DIVBYZERO);
			return -fp_bits_t::inf().get_val();
		}

		return static_cast<T>(ilogb_impl(x));
	}
} // namespace ccm::internal::impl
