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
	#ifdef CCMATH_HAS_SIMD_AVX
namespace ccm::intrin
{

	CCM_ALWAYS_INLINE simd<float, abi::avx> pow(simd<float, abi::avx> const & a, simd<float, abi::avx> const & b)
	{
		// NOLINTNEXTLINE(modernize-return-braced-init-list)
		return simd<float, abi::avx>(_mm256_pow_ps(a.get(), b.get()));
	}

	CCM_ALWAYS_INLINE simd<double, abi::avx> pow(simd<double, abi::avx> const & a, simd<double, abi::avx> const & b)
	{
		// NOLINTNEXTLINE(modernize-return-braced-init-list)
		return simd<double, abi::avx>(_mm256_pow_pd(a.get(), b.get()));
	}

} // namespace ccm::intrin

	#endif // CCMATH_HAS_SIMD_AVX
#endif	   // CCMATH_HAS_SIMD
