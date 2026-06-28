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

#include "ccmath/internal/math/generic/builtins/power/hypot.hpp"
#include "ccmath/internal/math/generic/func/power/hypot_gen.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true> [[nodiscard]] inline T hypot_rt(T x, T y) noexcept
	{
		if constexpr (ccm::builtin::has_runtime_hypot<T>)
		{
			return ccm::builtin::hypot_rt(x, y);
		} else
		{
			const auto scalar = [](T lhs, T rhs) { return gen::hypot_gen(lhs, rhs); };
			return simd_impl::binary_via_scalar_abi(x, y, scalar);
		}
	}
} // namespace ccm::rt
