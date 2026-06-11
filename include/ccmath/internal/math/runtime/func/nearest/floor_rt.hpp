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
#include "ccmath/internal/math/generic/builtins/nearest/floor.hpp"
#include "ccmath/internal/math/runtime/func/detail/trunc_scalar.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T floor_rt(T num) noexcept
	{
#if !defined(CCMATH_COMPILER_GCC)
		if constexpr (ccm::builtin::has_runtime_floor<T>) { return ccm::builtin::floor_rt(num); }
		else
#endif
		{
			return detail::floor_scalar(num);
		}
	}
} // namespace ccm::rt
