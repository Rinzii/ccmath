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
#include "ccmath/internal/math/generic/func/trig/tan_gen.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	T tan_rt(T num) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_tan) || defined(__builtin_tan)
		if constexpr (std::is_same_v<T, float>) { return __builtin_tanf(num); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_tan(num); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_tanl(num); }
		else { return static_cast<T>(__builtin_tanl(static_cast<long double>(num))); }
#else
		return gen::tan_gen(num);
#endif
	}
} // namespace ccm::rt
