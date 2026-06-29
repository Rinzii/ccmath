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
#include "ccmath/internal/math/generic/builtins/nearest/round.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/support/fp/directional_rounding_utils.hpp"
#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/isnan.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true> [[nodiscard]] inline T round_rt(T num) noexcept
	{
#ifndef CCMATH_COMPILER_GCC
		if constexpr (ccm::builtin::has_runtime_round<T>)
		{
			return ccm::builtin::round_rt(num);
		} else
#endif
		{
			if (ccm::isinf(num) || num == static_cast<T>(0) || ccm::isnan(num))
			{
				return num;
			}
			constexpr int round_mode = static_cast<int>(ccm::support::fp::rounding_mode::eFE_TONEARESTFROMZERO);
			return ccm::support::fp::directional_round(num, round_mode);
		}
	}
} // namespace ccm::rt
