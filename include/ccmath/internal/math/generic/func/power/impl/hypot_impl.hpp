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

#include <limits>
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

		// Compute scale in double precision to avoid float rounding scale to exactly
		// 1.0 when ratio is tiny, which would make the overflow guard silently fail.
		const double ratio_d = static_cast<double>(ratio);
		const double scale_d = ccm::sqrt(1.0 + ratio_d * ratio_d);

		if (static_cast<double>(x) > static_cast<double>(std::numeric_limits<T>::max()) / scale_d)
		{
			return fp_bits_t::inf().get_val();
		}

		return x * static_cast<T>(scale_d);
	}
} // namespace ccm::internal::impl
