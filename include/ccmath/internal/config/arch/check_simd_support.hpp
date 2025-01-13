/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

/*
 * Currently identifiable SIMD instruction sets:
 * 		x86:
 * 			- SSE2
 * 			- SSE4.1
 * 			- SSE4.2
 * 			- AVX2
 *
 * 		ARM:
 * 			- NEON
 */

#pragma once

#ifdef CCM_CONFIG_USE_RT_SIMD

// Streaming SIMD Extensions 2 (SSE2)
	#if defined(__SSE2__) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2) || defined(CCM_CONFIG_RT_SIMD_HAS_SSE2)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_SSE2 1
	#endif

// Streaming SIMD Extensions 4.1 (SSE4.1)
	#if defined(__SSE4_1__) || defined(CCM_CONFIG_RT_SIMD_HAS_SSE4_1)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_SSE4_1 1
	#endif

// Streaming SIMD Extensions 4.2 (SSE4.2)
	#if defined(__SSE4_2__) || defined(CCM_CONFIG_RT_SIMD_HAS_SSE4_2)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_SSE4_2 1
	#endif

// Streaming SIMD Extensions 4 (SSE4)
	#if (defined(CCMATH_HAS_SIMD_SSE4_1) && defined(CCMATH_HAS_SIMD_SSE4_2)) || defined(CCM_CONFIG_RT_SIMD_HAS_SSE4)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_SSE4 1
	#endif

// Advanced Vector Extensions 2 (AVX2)
	#if defined(__AVX2__) || defined(CCM_CONFIG_RT_SIMD_HAS_AVX2)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_AVX2 1
	#endif

// FMA (Fused Multiply-Add) Extensions
	#if defined(__FMA__) || defined(CCM_CONFIG_RT_SIMD_HAS_FMA)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_FMA 1
	#endif

// Intel Short Vector Math Library (SVML)
// As far as I am aware, there is no reliable way to detect SVML support at compile-time.
	#if defined(CCM_CONFIG_RT_SIMD_HAS_SVML)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_SVML 1
	#endif

// ARM Advanced SIMD (NEON)
	#if defined(__ARM_NEON) || defined(__ARM_NEON__) || defined(CCM_CONFIG_RT_SIMD_HAS_NEON)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_NEON 1
	#endif

// ARM Scalable Vector Extension (SVE)
/* TODO: At some point add proper SVE support.
	#if defined(__ARM_FEATURE_SVE)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD
		#endif
		#define CCMATH_HAS_SIMD_SVE 1
	#endif
 */
#endif // CCM_CONFIG_USE_RT_SIMD
