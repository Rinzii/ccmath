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

#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/predef/has_builtin.hpp"
#include "ccmath/math/trig/impl/inv_trig_impl.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T atan2_rt(T y, T x) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_atan2) || defined(__builtin_atan2)
		if constexpr (std::is_same_v<T, float>) { return __builtin_atan2f(y, x); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_atan2(y, x); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_atan2l(y, x); }
		else { return static_cast<T>(__builtin_atan2l(static_cast<long double>(y), static_cast<long double>(x))); }
#else
		const auto scalar = [](T yv, T xv)
		{
			return detail::dispatch_float_double(
				yv,
				[&](float yf) { return internal::impl::atan2_float(yf, static_cast<float>(xv)); },
				[&](double yd) { return internal::impl::atan2_double(yd, static_cast<double>(xv)); });
		};
		return simd_impl::binary_via_scalar_abi(y, x, scalar);
#endif
	}
} // namespace ccm::rt
