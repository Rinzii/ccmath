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

#include "ccmath/internal/math/generic/func/fmanip/impl/logb_impl.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/predef/has_builtin.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T logb_rt(T num) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_logb) || defined(__builtin_logb)
		if constexpr (std::is_same_v<T, float>) { return __builtin_logbf(num); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_logb(num); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_logbl(num); }
		else
		{
			return static_cast<T>(__builtin_logbl(static_cast<long double>(num)));
		}
#else
		return ccm::internal::impl::logb_impl(num);
#endif
	}
} // namespace ccm::rt
