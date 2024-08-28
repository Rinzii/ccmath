/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/runtime/simd/simd.hpp"

#ifdef CCMATH_HAS_SIMD
	#ifdef CCMATH_HAS_SIMD_SSE4
namespace ccm::intrin
{
	CCM_ALWAYS_INLINE simd<float, abi::sse4> sqrt(simd<float, abi::sse4> const & a)
	{
		// NOLINTNEXTLINE(modernize-return-braced-init-list)
		return simd<float, abi::sse4>(_mm_sqrt_ps(a.get()));
	}

	CCM_ALWAYS_INLINE simd<double, abi::sse4> sqrt(simd<double, abi::sse4> const & a)
	{
		// NOLINTNEXTLINE(modernize-return-braced-init-list)
		return simd<double, abi::sse4>(_mm_sqrt_pd(a.get()));
	}
} // namespace ccm::intrin

	#endif // CCMATH_HAS_SIMD_SSE2
#endif	   // CCM_CONFIG_USE_RT_SIMD