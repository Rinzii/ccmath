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

#include "ccmath/internal/math/generic/func/basic/max_gen.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/predef/has_builtin.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T fmax_rt(T x, T y) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_fmax) || defined(__builtin_fmax)
		if constexpr (std::is_same_v<T, float>) { return __builtin_fmaxf(x, y); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_fmax(x, y); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_fmaxl(x, y); }
		else
		{
			return static_cast<T>(__builtin_fmaxl(static_cast<long double>(x), static_cast<long double>(y)));
		}
#else
		return gen::max(x, y);
#endif
	}
} // namespace ccm::rt
