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
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/predef/has_builtin.hpp"
#include "ccmath/internal/support/fp/directional_rounding_utils.hpp"
#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/isnan.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T round_rt(T num) noexcept
	{
#if (CCM_HAS_BUILTIN(__builtin_round) || defined(__builtin_round)) && !defined(CCMATH_COMPILER_GCC)
		if constexpr (std::is_same_v<T, float>) { return __builtin_roundf(num); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_round(num); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_roundl(num); }
		else
		{
			return static_cast<T>(__builtin_roundl(static_cast<long double>(num)));
		}
#else
		if (ccm::isinf(num) || num == static_cast<T>(0) || ccm::isnan(num)) { return num; }
		constexpr int round_mode = static_cast<int>(ccm::support::fp::rounding_mode::eFE_TONEARESTFROMZERO);
		return ccm::support::fp::directional_round(num, round_mode);
#endif
	}
} // namespace ccm::rt
