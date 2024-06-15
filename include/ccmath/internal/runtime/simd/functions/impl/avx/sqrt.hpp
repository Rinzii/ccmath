/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/runtime/simd/simd.hpp"

#ifdef CCMATH_SIMD
	#ifdef CCMATH_SIMD_AVX
namespace ccm::intrin
{

	CCM_ALWAYS_INLINE inline simd<float, abi::avx> sqrt(simd<float, abi::avx> const & a)
	{
		// NOLINTNEXTLINE(modernize-return-braced-init-list)
		return simd<float, abi::avx>(_mm256_sqrt_ps(a.get()));
	}

	CCM_ALWAYS_INLINE inline simd<double, abi::avx> sqrt(simd<double, abi::avx> const & a)
	{
		// NOLINTNEXTLINE(modernize-return-braced-init-list)
		return simd<double, abi::avx>(_mm256_sqrt_pd(a.get()));
	}

} // namespace ccm::intrin

	#endif // CCMATH_SIMD_SSE2
#endif	   // CCM_CONFIG_USE_RT_SIMD