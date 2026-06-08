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

#include "ccmath/internal/config/compiler.hpp"
#include "ccmath/internal/math/runtime/func/detail/trunc_scalar.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/predef/has_builtin.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T trunc_rt(T num) noexcept
	{
#if (CCM_HAS_BUILTIN(__builtin_trunc) || defined(__builtin_trunc)) && !defined(CCMATH_COMPILER_GCC)
		if constexpr (std::is_same_v<T, float>) { return __builtin_truncf(num); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_trunc(num); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_truncl(num); }
		else
		{
			return static_cast<T>(__builtin_truncl(static_cast<long double>(num)));
		}
#else
		return detail::trunc_scalar(num);
#endif
	}
} // namespace ccm::rt
