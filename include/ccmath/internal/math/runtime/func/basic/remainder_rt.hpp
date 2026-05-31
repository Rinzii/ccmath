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

#include "ccmath/internal/math/runtime/func/detail/trunc_scalar.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/predef/has_builtin.hpp"
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"

#include <limits>
#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T remainder_rt(T x, T y) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_remainder) || defined(__builtin_remainder)
		if constexpr (std::is_same_v<T, float>) { return __builtin_remainderf(x, y); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_remainder(x, y); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_remainderl(x, y); }
		else { return static_cast<T>(__builtin_remainderl(static_cast<long double>(x), static_cast<long double>(y))); }
#else
		using FPBits_t = typename ccm::support::fp::FPBits<T>;
		const FPBits_t x_bits(x);
		const FPBits_t y_bits(y);
		const bool x_is_nan = x_bits.is_nan();
		const bool y_is_nan = y_bits.is_nan();
		if (CCM_UNLIKELY((x_bits.is_inf() && !y_is_nan) || (y_bits.is_zero() && !x_is_nan) || (x_is_nan || y_is_nan)))
		{
			return -std::numeric_limits<T>::quiet_NaN();
		}
		return static_cast<T>(x - (detail::trunc_scalar<T>(x / y) * y));
#endif
	}
} // namespace ccm::rt
