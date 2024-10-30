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
 * 			- SSE
 * 			- SSE2
 * 			- SSE3
 * 			- SSSE3
 * 			- SSE4.1
 * 			- SSE4.2
 * 			- AVX
 * 			- AVX2
 * 			- AVX512
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

// Streaming SIMD Extensions 3 (SSE3)
	#if defined(__SSE3__) || defined(CCM_CONFIG_RT_SIMD_HAS_SSE3)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_SSE3 1
	#endif

// Supplemental Streaming SIMD Extensions 3 (SSSE3)
	#if defined(__SSSE3__) || defined(CCM_CONFIG_RT_SIMD_HAS_SSSE3)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_SSSE3 1
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

// Advanced Vector Extensions (AVX)
	#if defined(__AVX__) || defined(CCM_CONFIG_RT_SIMD_HAS_AVX)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_AVX 1
	#endif

// Advanced Vector Extensions 2 (AVX2)
	#if defined(__AVX2__) || defined(CCM_CONFIG_RT_SIMD_HAS_AVX2)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_AVX2 1
	#endif

	// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Byte and Word instructions.
	#if defined(__AVX512BW__) || defined(CCM_CONFIG_RT_SIMD_HAS_AVX512BW)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_AVX512BW 1
	#endif

	// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Conflict Detection instructions.
	#if defined(__AVX512CD__) || defined(CCM_CONFIG_RT_SIMD_HAS_AVX512CD)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_AVX512CD 1
	#endif

	// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Doubleword and Quadword instructions.
	#if defined(__AVX512DQ__) || defined(CCM_CONFIG_RT_SIMD_HAS_AVX512DQ)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_AVX512DQ 1
	#endif

	// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Exponential and Reciprocal instructions.
	#if defined(__AVX512ER__) || defined(CCM_CONFIG_RT_SIMD_HAS_AVX512ER)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_AVX512ER 1
	#endif

	// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Foundation instructions.
	#if defined(__AVX512F__) || defined(CCM_CONFIG_RT_SIMD_HAS_AVX512F)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_AVX512F 1
	#endif

	// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Prefetch instructions.
	#if defined(__AVX512PF__) || defined(CCM_CONFIG_RT_SIMD_HAS_AVX512PF)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_AVX512PF 1
	#endif

	// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Vector Length extensions.
	#if defined(__AVX512VL__) || defined(CCM_CONFIG_RT_SIMD_HAS_AVX512VL)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_AVX512VL 1
	#endif

	// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Byte and Word instructions.
	#if (defined(__AVX512F__) && defined(__AVX512VL__) && defined(__AVX512BW__) && defined(__AVX512DQ__)) || defined(CCM_CONFIG_RT_SIMD_HAS_AVX512)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_AVX512 1
	#endif

// Intel Short Vector Math Library (SVML)
	#if defined(__FMA__) || defined(CCM_CONFIG_RT_SIMD_HAS_FMA)
		#ifndef CCMATH_HAS_SIMD
			#define CCMATH_HAS_SIMD 1
		#endif
		#define CCMATH_HAS_SIMD_FMA 1
	#endif

// Intel Short Vector Math Library (SVML)
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
