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

#include "ccmath/internal/math/generic/builtins/fmanip/nextafter.hpp"
#include "ccmath/internal/math/generic/func/fmanip/nextafter_gen.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T nextafter_rt(T x, T y) noexcept
	{
		if constexpr (ccm::builtin::has_runtime_nextafter<T>) { return ccm::builtin::nextafter_rt(x, y); }
		else
		{
			return gen::nextafter_gen(x, y);
		}
	}
} // namespace ccm::rt
