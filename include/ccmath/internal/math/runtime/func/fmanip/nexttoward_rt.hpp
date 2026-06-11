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

#include "ccmath/internal/math/generic/builtins/fmanip/nexttoward.hpp"
#include "ccmath/internal/math/generic/func/fmanip/nextafter_gen.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T nexttoward_rt(T x, long double y) noexcept
	{
		if constexpr (ccm::builtin::has_runtime_nexttoward<T>) { return ccm::builtin::nexttoward_rt(x, y); }
		else
		{
			return gen::nextafter_gen(x, y);
		}
	}
} // namespace ccm::rt
