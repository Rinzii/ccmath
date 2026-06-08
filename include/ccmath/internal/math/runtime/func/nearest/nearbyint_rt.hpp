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
#include "ccmath/internal/support/fenv/rounding_mode.hpp"
#include "ccmath/internal/support/fp/directional_rounding_utils.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T nearbyint_rt(T num) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_nearbyint) || defined(__builtin_nearbyint)
		if constexpr (std::is_same_v<T, float>) { return __builtin_nearbyintf(num); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_nearbyint(num); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_nearbyintl(num); }
		else
		{
			return static_cast<T>(__builtin_nearbyintl(static_cast<long double>(num)));
		}
#else
		return ccm::support::fp::directional_round(num, ccm::support::fenv::get_rounding_mode());
#endif
	}
} // namespace ccm::rt
