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
#include "ccmath/internal/math/generic/builtins/nearest/rint.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/support/fenv/rounding_mode.hpp"
#include "ccmath/internal/support/fp/directional_rounding_utils.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T rint_rt(T num) noexcept
	{
#ifdef CCMATH_COMPILER_GCC
		if constexpr (ccm::builtin::has_runtime_rint<T> && std::is_same_v<T, float>) { return ccm::builtin::rint_rt(num); }
		else
		{
			return ccm::support::fp::directional_round(num, ccm::support::fenv::get_rounding_mode());
		}
#else
		if constexpr (ccm::builtin::has_runtime_rint<T>) { return ccm::builtin::rint_rt(num); }
		else
		{
			return ccm::support::fp::directional_round(num, ccm::support::fenv::get_rounding_mode());
		}
#endif
	}
} // namespace ccm::rt
