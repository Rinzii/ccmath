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

#include "ccmath/internal/math/runtime/simd/simd.hpp"

namespace ccm::intrin
{
	template <class T, class Abi, typename Fn>
	CCM_ALWAYS_INLINE simd<T, Abi> map_scalar(simd<T, Abi> const & input, Fn fn)
	{
		return simd<T, Abi>(fn(input.convert()));
	}

	template <class T, class Abi, typename Fn>
	CCM_ALWAYS_INLINE simd<T, Abi> map_scalar(simd<T, Abi> const & lhs, simd<T, Abi> const & rhs, Fn fn)
	{
		return simd<T, Abi>(fn(lhs.convert(), rhs.convert()));
	}
} // namespace ccm::intrin
