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

#include "ccmath/internal/math/generic/builtins/trig/tan.hpp"
#include "ccmath/internal/math/generic/func/trig/tan_gen.hpp"
#include "ccmath/internal/math/runtime/func/detail/msvc_libm.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/math/runtime/func/svml_dispatch.hpp"
#include "ccmath/internal/math/runtime/simd/func/catalog.hpp"

#include <type_traits>

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] inline T tan_rt(T num) noexcept
	{
#if defined(_MSC_VER) && !defined(__clang__)
		return detail::msvc_libm::tan_call(num);
#else
		if constexpr (ccm::builtin::has_runtime_tan<T>) { return ccm::builtin::tan_rt(num); }
		else
		{
			const auto scalar = [](T value) { return gen::tan_gen(value); };
	#if defined(CCMATH_HAS_SIMD) && defined(CCMATH_HAS_SIMD_SVML) && !defined(_MSC_VER)
			return detail::unary_trig_svml_or_impl(num, [](auto v) { return intrin::tan(v); }, scalar);
	#else
			return simd_impl::unary_via_scalar_abi(num, scalar);
	#endif
		}
#endif
	}
} // namespace ccm::rt
