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
#include "ccmath/math/fmanip/impl/ilogb_impl.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline int ilogb_rt(T num) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_ilogb) || defined(__builtin_ilogb)
		if constexpr (std::is_same_v<T, float>) { return __builtin_ilogbf(num); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_ilogb(num); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_ilogbl(num); }
		else { return __builtin_ilogbl(static_cast<long double>(num)); }
#else
		return internal::impl::ilogb_impl(num);
#endif
	}
} // namespace ccm::rt
