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
#include "ccmath/math/basic/fabs.hpp"
#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/math/expo/exp.hpp"
#include "ccmath/math/expo/log.hpp"

#include <limits>
#include <type_traits>

namespace ccm::internal::impl
{
	template <typename T> constexpr T cbrt_impl(T x) noexcept
	{
		using fp_bits_t = ccm::support::fp::FPBits<T>;

		fp_bits_t const bits(x);

		if (CCM_UNLIKELY(bits.is_nan()))
		{
			return x;
		}

		if (CCM_UNLIKELY(bits.is_inf()))
		{
			return x;
		}

		if (CCM_UNLIKELY(bits.is_zero()))
		{
			return x;
		}

		const bool negative = bits.is_neg();
		const T ax			= ccm::fabs(x);

		T y			 = ccm::exp(ccm::log(ax) / static_cast<T>(3));
		const T y_sq = y * y;
		y			 = ((static_cast<T>(2) * y) + (ax / y_sq)) / static_cast<T>(3);

		return negative ? -y : y;
	}
} // namespace ccm::internal::impl
