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

		#ifdef CCMATH_HAS_SIMD_SVML
			#include <immintrin.h>
		#else
			#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"
		#endif

namespace ccm::intrin
{
	CCM_ALWAYS_INLINE simd<float, abi::sse2> pow(simd<float, abi::sse2> const & a, simd<float, abi::sse2> const & b)
	{
		// The cmake performs a test validating if the compiler supports SVML.
		// As far as I'm aware, this is the only reliable way to check.
		#ifdef CCMATH_HAS_SIMD_SVML
		return { _mm_pow_ps(a.get(), b.get()) };
		#else
		// TODO: Replace this with a refined solution. For the time being this is temporary.
		return { gen::pow_gen(a.convert(), b.convert()) };
		#endif
	}

	CCM_ALWAYS_INLINE simd<double, abi::sse2> pow(simd<double, abi::sse2> const & a, simd<double, abi::sse2> const & b)
	{
		// The cmake performs a test validating if the compiler supports SVML.
		// As far as I'm aware, this is the only reliable way to check.
		#ifdef CCMATH_HAS_SIMD_SVML
		return { _mm_pow_pd(a.get(), b.get()) };
		#else
		// TODO: Replace this with a refined solution. For the time being this is temporary.
		return { gen::pow_gen(a.convert(), b.convert()) };
		#endif
	}
} // namespace ccm::intrin

	#endif // CCMATH_HAS_SIMD_SSE2
#endif	   // CCMATH_HAS_SIMD
