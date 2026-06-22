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

#include "ccmath/internal/math/generic/builtins/trig/sin.hpp"
#include "ccmath/internal/math/generic/func/trig/sin_gen.hpp"
#include "ccmath/internal/math/runtime/func/detail/system_math.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/math/runtime/func/svml_dispatch.hpp"
#include "ccmath/internal/math/runtime/simd/func/catalog.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T sin_rt(T num) noexcept
	{
#if defined(CCM_CONFIG_SYSTEM_MATH)
		return detail::sys::sin_call(num);
#else
		if constexpr (ccm::builtin::has_runtime_sin<T>) { return ccm::builtin::sin_rt(num); }
		else
		{
			const auto scalar = [](T value) { return gen::sin_gen(value); };
	#if defined(CCMATH_HAS_SIMD) && defined(CCMATH_HAS_SIMD_SVML) && !defined(_MSC_VER)
			return detail::unary_trig_svml_or_impl(num, [](auto v) { return intrin::sin(v); }, scalar);
	#else
			return simd_impl::unary_via_scalar_abi(num, scalar);
	#endif
		}
#endif
	}
} // namespace ccm::rt
