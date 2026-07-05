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

namespace ccm::rt::detail
{
#ifdef CCMATH_HAS_SIMD_SVML
	template <typename T> inline constexpr bool has_svml_unary_v = simd_eligible_v<T>;
#else
	template <typename T> inline constexpr bool has_svml_unary_v = false;
#endif

	template <typename T, typename SimdOp, typename ScalarFn> [[nodiscard]] inline T unary_svml_or_impl(T value, SimdOp simd_op, ScalarFn scalar_fn) noexcept
	{
		if constexpr (has_svml_unary_v<T>)
		{
			return simd_impl::unary(value, simd_op, scalar_fn);
		} else
		{
			return simd_impl::unary_via_scalar_abi(value, scalar_fn);
		}
	}

	template <typename T, typename SimdOp, typename ScalarFn>
	[[nodiscard]] inline T unary_trig_svml_or_impl(T value, SimdOp simd_op, ScalarFn scalar_fn) noexcept
	{
		return unary_svml_or_impl(value, simd_op, scalar_fn);
	}
} // namespace ccm::rt::detail
