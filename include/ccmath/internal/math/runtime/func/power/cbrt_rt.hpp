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

#include "ccmath/internal/math/generic/builtins/power/cbrt.hpp"
#include "ccmath/internal/math/generic/func/power/cbrt_gen.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true> [[nodiscard]] inline T cbrt_rt(T num) noexcept
	{
		if constexpr (ccm::builtin::has_runtime_cbrt<T>)
		{
			return ccm::builtin::cbrt_rt(num);
		} else
		{
			const auto scalar = [](T value) { return gen::cbrt_gen(value); };
			return simd_impl::unary_via_scalar_abi(num, scalar);
		}
	}
} // namespace ccm::rt
