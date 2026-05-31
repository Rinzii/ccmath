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

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T fma_rt(T x, T y, T z) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_fma) || defined(__builtin_fma)
		if constexpr (std::is_same_v<T, float>) { return __builtin_fmaf(x, y, z); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_fma(x, y, z); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_fmal(x, y, z); }
		else { return static_cast<T>(__builtin_fmal(static_cast<long double>(x), static_cast<long double>(y), static_cast<long double>(z))); }
#else
		return (x * y) + z;
#endif
	}
} // namespace ccm::rt
