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

#include "ccmath/internal/math/generic/func/power/sqrt_gen.hpp"
#include "ccmath/internal/math/runtime/simd/simd.hpp"

#ifdef CCMATH_HAS_SIMD
	#ifdef CCMATH_HAS_SIMD_AVX2
namespace ccm::intrin
{

	CCM_ALWAYS_INLINE simd<float, abi::avx2> pow(simd<float, abi::avx2> const & a, simd<float, abi::avx2> const & b)
	{
		#ifdef CCMATH_HAS_SIMD_SVML
		return simd<float, abi::avx2>(_mm256_pow_ps(a.get(), b.get()));
		#else
		return simd<float, abi::avx2>(gen::pow_gen(a.convert(), b.convert()));
		#endif
	}

	CCM_ALWAYS_INLINE simd<double, abi::avx2> pow(simd<double, abi::avx2> const & a, simd<double, abi::avx2> const & b)
	{
		#ifdef CCMATH_HAS_SIMD_SVML
		return simd<double, abi::avx2>(_mm256_pow_pd(a.get(), b.get()));
		#else
		return simd<double, abi::avx2>(gen::pow_gen(a.convert(), b.convert()));
		#endif
	}

} // namespace ccm::intrin

	#endif // CCMATH_HAS_SIMD_AVX2
#endif	   // CCMATH_HAS_SIMD
