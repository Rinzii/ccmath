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

#ifdef CCMATH_HAS_SIMD
	#ifdef CCMATH_HAS_SIMD_SSE2
namespace ccm::intrin
{
	CCM_ALWAYS_INLINE simd<float, abi::sse2> sqrt(simd<float, abi::sse2> const & a)
	{
		// NOLINTNEXTLINE(modernize-return-braced-init-list)
		return simd<float, abi::sse2>(_mm_sqrt_ps(a.get()));
	}

	CCM_ALWAYS_INLINE simd<double, abi::sse2> sqrt(simd<double, abi::sse2> const & a)
	{
		// NOLINTNEXTLINE(modernize-return-braced-init-list)
		return simd<double, abi::sse2>(_mm_sqrt_pd(a.get()));
	}
} // namespace ccm::intrin

	#endif // CCMATH_HAS_SIMD_SSE2
#endif	   // CCMATH_HAS_SIMD
