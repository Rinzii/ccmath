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

#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/predef/has_builtin.hpp"
#include "ccmath/internal/support/helpers/internal_ldexp.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T ldexp_rt(T num, int exp) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_ldexp) || defined(__builtin_ldexp)
		if constexpr (std::is_same_v<T, float>) { return __builtin_ldexpf(num, exp); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_ldexp(num, exp); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_ldexpl(num, exp); }
		else { return static_cast<T>(__builtin_ldexpl(static_cast<long double>(num), exp)); }
#else
		return ccm::support::helpers::internal_ldexp(num, exp);
#endif
	}
} // namespace ccm::rt
