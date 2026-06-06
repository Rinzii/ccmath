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
#include "ccmath/math/power/impl/hypot_impl.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T hypot_rt(T x, T y) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_hypot) || defined(__builtin_hypot)
		if constexpr (std::is_same_v<T, float>) { return __builtin_hypotf(x, y); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_hypot(x, y); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_hypotl(x, y); }
		else { return static_cast<T>(__builtin_hypotl(static_cast<long double>(x), static_cast<long double>(y))); }
#else
		const auto scalar = [](T lhs, T rhs) { return internal::impl::hypot_impl(lhs, rhs); };
		return simd_impl::binary_via_scalar_abi(x, y, scalar);
#endif
	}
} // namespace ccm::rt
