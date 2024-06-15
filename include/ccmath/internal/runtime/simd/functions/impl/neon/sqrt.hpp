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
	#ifdef CCMATH_SIMD_NEON
namespace ccm::intrin
{
	CCM_ALWAYS_INLINE inline simd<float, abi::neon> sqrt(simd<float, abi::neon> const & a)
	{
		// NOLINTNEXTLINE(modernize-return-braced-init-list)
		return simd<float, abi::neon>(vsqrtq_f32(a.get()));
	}

	CCM_ALWAYS_INLINE inline simd<double, abi::neon> sqrt(simd<double, abi::neon> const & a)
	{
		// NOLINTNEXTLINE(modernize-return-braced-init-list)
		return simd<double, abi::neon>(vsqrtq_f64(a.get()));
	}
} // namespace ccm::intrin

	#endif // CCMATH_SIMD_NEON
#endif	   // CCM_CONFIG_USE_RT_SIMD