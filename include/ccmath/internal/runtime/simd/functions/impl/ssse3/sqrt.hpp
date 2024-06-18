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
	#ifdef CCMATH_HAS_SIMD_SSSE3
namespace ccm::intrin
{
	CCM_ALWAYS_INLINE inline simd<float, abi::ssse3> sqrt(simd<float, abi::ssse3> const & a)
	{
		// NOLINTNEXTLINE(modernize-return-braced-init-list)
		return simd<float, abi::ssse3>(_mm_sqrt_ps(a.get()));
	}

	CCM_ALWAYS_INLINE inline simd<double, abi::ssse3> sqrt(simd<double, abi::ssse3> const & a)
	{
		// NOLINTNEXTLINE(modernize-return-braced-init-list)
		return simd<double, abi::ssse3>(_mm_sqrt_pd(a.get()));
	}
} // namespace ccm::intrin

	#endif // CCMATH_HAS_SIMD_SSE2
#endif	   // CCM_CONFIG_USE_RT_SIMD