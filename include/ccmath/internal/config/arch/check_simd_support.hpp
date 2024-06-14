/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

/*
 * Currently identifiable SIMD instruction sets:
 * 		x86:
 * 			- SSE
 * 			- AVX
 *
 * 		ARM:
 * 			- NEON (TODO)
 */

#pragma once

#ifdef CCM_CONFIG_USE_RT_SIMD

// Identify if we are allowing simd instructions.
#if defined(__SSE__) || (defined(_M_IX86_FP) && _M_IX86_FP >= 1)
	#define CCMATH_SIMD_SSE 1
#endif

#if defined(__SSE2__) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
	#define CCMATH_SIMD_SSE2 1
#endif

#if defined(__SSE3__)
	#define CCMATH_SIMD_SSE3 1
#endif

#if defined(__SSSE3__)
	#define CCMATH_SIMD_SSSE3 1
#endif

#if defined(__SSE4_1__)
	#define CCMATH_SIMD_SSE4_1 1
#endif

#if defined(__SSE4_2__)
	#define CCMATH_SIMD_SSE4_2 1
#endif

#if defined(__AVX__)
	#define CCMATH_SIMD_AVX 1
#endif

#if defined(__AVX2__)
	#define CCMATH_SIMD_AVX2 1
#endif

// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Byte and Word instructions.
#if defined(__AVX512BW__)
	#define CCMATH_SIMD_AVX512BW 1
#endif

// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Conflict Detection instructions.
#if defined(__AVX512CD__)
	#define CCMATH_SIMD_AVX512CD 1
#endif

// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Doubleword and Quadword instructions.
#if defined(__AVX512DQ__)
	#define CCMATH_SIMD_AVX512DQ 1
#endif

// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Exponential and Reciprocal instructions.
#if defined(__AVX512ER__)
	#define CCMATH_SIMD_AVX512ER 1
#endif

// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Foundation instructions.
#if defined(__AVX512F__)
	#define CCMATH_SIMD_AVX512F 1
#endif

// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Prefetch instructions.
#if defined(__AVX512PF__)
	#define CCMATH_SIMD_AVX512PF 1
#endif

// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Vector Length extensions.
#if defined(__AVX512VL__)
	#define CCMATH_SIMD_AVX512VL 1
#endif

#if defined(__AVX512F__) && defined(__AVX512VL__) && defined(__AVX512BW__) && defined(__AVX512DQ__)
	#define CCMATH_SIMD_AVX512 1
#endif

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
	#define CCMATH_SIMD_NEON 1
#endif

// If we have any simd instructions, then we define CCMATH_SIMD so that we can use it in our code.
#if defined(CCMATH_SIMD_SSE) || defined(CCMATH_SIMD_SSE2) || defined(CCMATH_SIMD_SSE3) || defined(CCMATH_SIMD_SSSE3) || defined(CCMATH_SIMD_SSE4_1) ||                    \
	defined(CCMATH_SIMD_SSE4_2) || defined(CCMATH_SIMD_AVX) || defined(CCMATH_SIMD_AVX2) || defined(CCMATH_SIMD_AVX512BW) || defined(CCMATH_SIMD_AVX512CD) ||             \
	defined(CCMATH_SIMD_AVX512DQ) || defined(CCMATH_SIMD_AVX512ER) || defined(CCMATH_SIMD_AVX512F) || defined(CCMATH_SIMD_AVX512PF) || defined(CCMATH_SIMD_AVX512VL) ||   \
	defined(CCMATH_SIMD_NEON)
	#ifdef CCMATH_SIMD
		#undef CCMATH_SIMD
	#endif
	#define CCMATH_SIMD 1
#endif

#endif // CCM_CONFIG_USE_RT_SIMD