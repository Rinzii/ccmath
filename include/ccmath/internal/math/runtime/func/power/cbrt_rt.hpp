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

#include "ccmath/internal/math/generic/func/power/impl/cbrt_impl.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/predef/has_builtin.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T cbrt_rt(T num) noexcept
	{
#if CCM_HAS_BUILTIN(__builtin_cbrt) || defined(__builtin_cbrt)
		if constexpr (std::is_same_v<T, float>) { return __builtin_cbrtf(num); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_cbrt(num); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_cbrtl(num); }
		else
		{
			return static_cast<T>(__builtin_cbrtl(static_cast<long double>(num)));
		}
#else
		const auto scalar = [](T value) { return internal::impl::cbrt_impl(value); };
		return simd_impl::unary_via_scalar_abi(num, scalar);
#endif
	}
} // namespace ccm::rt
