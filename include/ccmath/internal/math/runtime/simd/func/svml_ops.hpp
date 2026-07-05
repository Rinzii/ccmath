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

#include "ccmath/internal/config/arch/check_simd_support.hpp"
#include "ccmath/internal/math/runtime/simd/simd.hpp"

#ifdef CCMATH_HAS_SIMD
	#ifdef CCMATH_HAS_SIMD_SVML
		#include <immintrin.h>

		#ifdef CCMATH_HAS_SIMD_SSE2
namespace ccm::intrin
{
	CCM_ALWAYS_INLINE simd<float, abi::sse2> sin(simd<float, abi::sse2> const & a)
	{
		return simd<float, abi::sse2>(_mm_sin_ps(a.get()));
	}

	CCM_ALWAYS_INLINE simd<double, abi::sse2> sin(simd<double, abi::sse2> const & a)
	{
		return simd<double, abi::sse2>(_mm_sin_pd(a.get()));
	}

	CCM_ALWAYS_INLINE simd<float, abi::sse2> cos(simd<float, abi::sse2> const & a)
	{
		return simd<float, abi::sse2>(_mm_cos_ps(a.get()));
	}

	CCM_ALWAYS_INLINE simd<double, abi::sse2> cos(simd<double, abi::sse2> const & a)
	{
		return simd<double, abi::sse2>(_mm_cos_pd(a.get()));
	}

	CCM_ALWAYS_INLINE simd<float, abi::sse2> tan(simd<float, abi::sse2> const & a)
	{
		return simd<float, abi::sse2>(_mm_tan_ps(a.get()));
	}

	CCM_ALWAYS_INLINE simd<double, abi::sse2> tan(simd<double, abi::sse2> const & a)
	{
		return simd<double, abi::sse2>(_mm_tan_pd(a.get()));
	}

	CCM_ALWAYS_INLINE simd<float, abi::sse2> exp(simd<float, abi::sse2> const & a)
	{
		return simd<float, abi::sse2>(_mm_exp_ps(a.get()));
	}

	CCM_ALWAYS_INLINE simd<double, abi::sse2> exp(simd<double, abi::sse2> const & a)
	{
		return simd<double, abi::sse2>(_mm_exp_pd(a.get()));
	}

	CCM_ALWAYS_INLINE simd<float, abi::sse2> log(simd<float, abi::sse2> const & a)
	{
		return simd<float, abi::sse2>(_mm_log_ps(a.get()));
	}

	CCM_ALWAYS_INLINE simd<double, abi::sse2> log(simd<double, abi::sse2> const & a)
	{
		return simd<double, abi::sse2>(_mm_log_pd(a.get()));
	}

	CCM_ALWAYS_INLINE simd<float, abi::sse2> log10(simd<float, abi::sse2> const & a)
	{
		return simd<float, abi::sse2>(_mm_log10_ps(a.get()));
	}

	CCM_ALWAYS_INLINE simd<double, abi::sse2> log10(simd<double, abi::sse2> const & a)
	{
		return simd<double, abi::sse2>(_mm_log10_pd(a.get()));
	}

	CCM_ALWAYS_INLINE simd<float, abi::sse2> log2(simd<float, abi::sse2> const & a)
	{
		return simd<float, abi::sse2>(_mm_log2_ps(a.get()));
	}

	CCM_ALWAYS_INLINE simd<double, abi::sse2> log2(simd<double, abi::sse2> const & a)
	{
		return simd<double, abi::sse2>(_mm_log2_pd(a.get()));
	}
} // namespace ccm::intrin
		#endif

		#ifdef CCMATH_HAS_SIMD_AVX2
namespace ccm::intrin
{
	CCM_ALWAYS_INLINE simd<float, abi::avx2> sin(simd<float, abi::avx2> const & a)
	{
		return simd<float, abi::avx2>(_mm256_sin_ps(a.get()));
	}

	CCM_ALWAYS_INLINE simd<double, abi::avx2> sin(simd<double, abi::avx2> const & a)
	{
		return simd<double, abi::avx2>(_mm256_sin_pd(a.get()));
	}

	CCM_ALWAYS_INLINE simd<float, abi::avx2> cos(simd<float, abi::avx2> const & a)
	{
		return simd<float, abi::avx2>(_mm256_cos_ps(a.get()));
	}

	CCM_ALWAYS_INLINE simd<double, abi::avx2> cos(simd<double, abi::avx2> const & a)
	{
		return simd<double, abi::avx2>(_mm256_cos_pd(a.get()));
	}

	CCM_ALWAYS_INLINE simd<float, abi::avx2> tan(simd<float, abi::avx2> const & a)
	{
		return simd<float, abi::avx2>(_mm256_tan_ps(a.get()));
	}

	CCM_ALWAYS_INLINE simd<double, abi::avx2> tan(simd<double, abi::avx2> const & a)
	{
		return simd<double, abi::avx2>(_mm256_tan_pd(a.get()));
	}

	CCM_ALWAYS_INLINE simd<float, abi::avx2> exp(simd<float, abi::avx2> const & a)
	{
		return simd<float, abi::avx2>(_mm256_exp_ps(a.get()));
	}

	CCM_ALWAYS_INLINE simd<double, abi::avx2> exp(simd<double, abi::avx2> const & a)
	{
		return simd<double, abi::avx2>(_mm256_exp_pd(a.get()));
	}

	CCM_ALWAYS_INLINE simd<float, abi::avx2> log(simd<float, abi::avx2> const & a)
	{
		return simd<float, abi::avx2>(_mm256_log_ps(a.get()));
	}

	CCM_ALWAYS_INLINE simd<double, abi::avx2> log(simd<double, abi::avx2> const & a)
	{
		return simd<double, abi::avx2>(_mm256_log_pd(a.get()));
	}

	CCM_ALWAYS_INLINE simd<float, abi::avx2> log10(simd<float, abi::avx2> const & a)
	{
		return simd<float, abi::avx2>(_mm256_log10_ps(a.get()));
	}

	CCM_ALWAYS_INLINE simd<double, abi::avx2> log10(simd<double, abi::avx2> const & a)
	{
		return simd<double, abi::avx2>(_mm256_log10_pd(a.get()));
	}

	CCM_ALWAYS_INLINE simd<float, abi::avx2> log2(simd<float, abi::avx2> const & a)
	{
		return simd<float, abi::avx2>(_mm256_log2_ps(a.get()));
	}

	CCM_ALWAYS_INLINE simd<double, abi::avx2> log2(simd<double, abi::avx2> const & a)
	{
		return simd<double, abi::avx2>(_mm256_log2_pd(a.get()));
	}
} // namespace ccm::intrin
		#endif
	#endif
#endif
