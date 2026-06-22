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

#include "ccmath/internal/math/generic/builtins/fmanip/logb.hpp"
#include "ccmath/internal/math/generic/func/fmanip/impl/logb_impl.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T logb_rt(T num) noexcept
	{
		if constexpr (ccm::builtin::has_runtime_logb<T>) { return ccm::builtin::logb_rt(num); }
		else
		{
			return ccm::internal::impl::logb_impl(num);
		}
	}
} // namespace ccm::rt
