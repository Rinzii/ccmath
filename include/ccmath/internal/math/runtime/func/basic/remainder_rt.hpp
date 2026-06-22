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

#include "ccmath/internal/math/generic/builtins/basic/remainder.hpp"
#include "ccmath/internal/math/runtime/func/detail/system_math.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/math/basic/remquo.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T remainder_rt(T x, T y) noexcept
	{
#if defined(CCM_CONFIG_SYSTEM_MATH)
		return detail::sys::remainder_call(x, y);
#else
		if constexpr (ccm::builtin::has_runtime_remainder<T>) { return ccm::builtin::remainder_rt(x, y); }
		else
		{
			// No builtin and no system math, so reuse the exact remquo reduction. It rounds the
			// quotient to nearest, ties-to-even, and covers the special cases, unlike the trunc
			// formula that the other no-builtin basic fallbacks share.
			int quotient = 0;
			return ccm::remquo<T>(x, y, &quotient);
		}
#endif
	}
} // namespace ccm::rt
