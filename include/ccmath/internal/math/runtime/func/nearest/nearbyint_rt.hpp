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

#include "ccmath/internal/math/generic/builtins/nearest/nearbyint.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/support/fenv/rounding_mode.hpp"
#include "ccmath/internal/support/fp/directional_rounding_utils.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true> [[nodiscard]] inline T nearbyint_rt(T num) noexcept
	{
		if constexpr (ccm::builtin::has_runtime_nearbyint<T>)
		{
			return ccm::builtin::nearbyint_rt(num);
		} else
		{
			return ccm::support::fp::directional_round(num, ccm::support::fenv::get_rounding_mode());
		}
	}
} // namespace ccm::rt
