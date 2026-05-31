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

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T ceil_rt(T num) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_ceil) || defined(__builtin_ceil)
		if constexpr (std::is_same_v<T, float>) { return __builtin_ceilf(num); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_ceil(num); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_ceill(num); }
		else { return static_cast<T>(__builtin_ceill(static_cast<long double>(num))); }
#else
		return detail::ceil_scalar(num);
#endif
	}
} // namespace ccm::rt
