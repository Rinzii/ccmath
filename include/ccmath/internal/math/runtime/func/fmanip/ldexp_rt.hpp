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

#include "ccmath/internal/math/generic/builtins/fmanip/ldexp.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/support/helpers/internal_ldexp.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T ldexp_rt(T num, int exp) noexcept
	{
		if constexpr (ccm::builtin::has_runtime_ldexp<T>) { return ccm::builtin::ldexp_rt(num, exp); }
		else
		{
			return ccm::support::helpers::internal_ldexp(num, exp);
		}
	}
} // namespace ccm::rt
