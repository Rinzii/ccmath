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
#include "ccmath/math/power/sqrt.hpp"

#include <type_traits>

namespace ccm::internal::impl
{
	template <typename T>
	constexpr T hypot_impl(T x, T y) noexcept
	{
		using fp_bits_t = ccm::support::fp::FPBits<T>;

		if (CCM_UNLIKELY(ccm::isinf(x) || ccm::isinf(y))) { return fp_bits_t::inf().get_val(); }

		if (CCM_UNLIKELY(ccm::isnan(x))) { return x + y; }
		if (CCM_UNLIKELY(ccm::isnan(y))) { return x + y; }

		x = ccm::fabs(x);
		y = ccm::fabs(y);

		if (CCM_UNLIKELY(x == static_cast<T>(0) && y == static_cast<T>(0))) { return static_cast<T>(0); }

		if (y > x)
		{
			const T tmp = x;
			x			= y;
			y			= tmp;
		}

		const T ratio = y / x;
		return x * ccm::sqrt(static_cast<T>(1) + (ratio * ratio));
	}
} // namespace ccm::internal::impl
