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
 * 			- SSE2
 * 			- SSE3
 * 			- SSSE3
 * 			- SSE4.1
 * 			- SSE4.2
 * 			- AVX
 * 			- AVX2
 * 			- AVX-512 BW
 * 			- AVX-512 CD
 * 			- AVX-512 DQ
 * 			- AVX-512 ER
 * 			- AVX-512 F
 * 			- AVX-512 PF
 * 			- AVX-512 VL
 *
 * 		ARM:
 * 			- NEON
 */

#pragma once

#if defined(__SSE__) || (defined(_M_IX86_FP) && _M_IX86_FP >= 1)
	#define CCM_HAS_SIMD_SSE 1
#endif

#if defined(__SSE2__) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
	#define CCM_HAS_SIMD_SSE2 1
#endif

#if defined(__SSE3__)
	#define CCM_HAS_SIMD_SSE3 1
#endif

#if defined(__SSSE3__)
	#define CCM_HAS_SIMD_SSSE3 1
#endif

#if defined(__SSE4_1__)
	#define CCM_HAS_SIMD_SSE4_1 1
#endif

#if defined(__SSE4_2__)
	#define CCM_HAS_SIMD_SSE4_2 1
#endif

#if defined(__AVX__)
	#define CCM_HAS_SIMD_AVX 1
#endif

#if defined(__AVX2__)
	#define CCM_HAS_SIMD_AVX2 1
#endif

// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Byte and Word instructions.
#if defined(__AVX512BW__)
	#define CCM_HAS_SIMD_AVX512BW 1
#endif

// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Conflict Detection instructions.
#if defined(__AVX512CD__)
	#define CCM_HAS_SIMD_AVX512CD 1
#endif

// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Doubleword and Quadword instructions.
#if defined(__AVX512DQ__)
	#define CCM_HAS_SIMD_AVX512DQ 1
#endif

// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Exponential and Reciprocal instructions.
#if defined(__AVX512ER__)
	#define CCM_HAS_SIMD_AVX512ER 1
#endif

// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Foundation instructions.
#if defined(__AVX512F__)
	#define CCM_HAS_SIMD_AVX512F 1
#endif

// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Prefetch instructions.
#if defined(__AVX512PF__)
	#define CCM_HAS_SIMD_AVX512PF 1
#endif

// Processors that support Intel Advanced Vector Extensions 512 (Intel AVX-512) Vector Length extensions.
#if defined(__AVX512VL__)
	#define CCM_HAS_SIMD_AVX512VL 1
#endif

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
	#define CCM_HAS_SIMD_NEON 1
#endif

// If we have any simd instructions, then we define CCM_HAS_SIMD so that we can use it in our code.
#if defined(CCM_HAS_SIMD_SSE) || defined(CCM_HAS_SIMD_SSE2) || defined(CCM_HAS_SIMD_SSE3) || defined(CCM_HAS_SIMD_SSSE3) || defined(CCM_HAS_SIMD_SSE4_1) ||    \
	defined(CCM_HAS_SIMD_SSE4_2) || defined(CCM_HAS_SIMD_AVX) || defined(CCM_HAS_SIMD_AVX2) || defined(CCM_HAS_SIMD_AVX512BW) ||                               \
	defined(CCM_HAS_SIMD_AVX512CD) || defined(CCM_HAS_SIMD_AVX512DQ) || defined(CCM_HAS_SIMD_AVX512ER) || defined(CCM_HAS_SIMD_AVX512F) ||                     \
	defined(CCM_HAS_SIMD_AVX512PF) || defined(CCM_HAS_SIMD_AVX512VL) || defined(CCM_HAS_SIMD_NEON)
	#define CCM_HAS_SIMD 1
#else
	#undef CCM_HAS_SIMD
#endif
