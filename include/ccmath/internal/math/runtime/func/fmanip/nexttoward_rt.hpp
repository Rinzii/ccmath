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

#include "ccmath/internal/math/generic/func/fmanip/nextafter_gen.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/predef/has_builtin.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T nexttoward_rt(T x, long double y) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_nexttoward) || defined(__builtin_nexttoward)
		if constexpr (std::is_same_v<T, float>) { return __builtin_nexttowardf(x, y); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_nexttoward(x, y); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_nexttowardl(x, y); }
		else { return static_cast<T>(__builtin_nexttowardl(static_cast<long double>(x), y)); }
#else
		return gen::nextafter_gen(x, y);
#endif
	}
} // namespace ccm::rt
