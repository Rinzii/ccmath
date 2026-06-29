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

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/math/nearest/trunc.hpp"

namespace ccm::internal::impl
{
	template <typename T> constexpr T modf_impl(T x, T * iptr) noexcept
	{
		using fp_bits_t = ccm::support::fp::FPBits<T>;

		if (CCM_UNLIKELY(ccm::isnan(x)))
		{
			*iptr = x;
			return x;
		}

		if (CCM_UNLIKELY(ccm::isinf(x)))
		{
			*iptr = x;
			return fp_bits_t::zero(fp_bits_t(x).sign()).get_val();
		}

		if (CCM_UNLIKELY(x == T{}))
		{
			*iptr = x;
			return x;
		}

		*iptr		 = ccm::trunc(x);
		T fractional = x - *iptr;

		if (fractional == T{})
		{
			fractional = fp_bits_t::zero(fp_bits_t(x).sign()).get_val();
		}

		return fractional;
	}
} // namespace ccm::internal::impl
