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

// TODO: Add support for ARM NEON

	#if defined(__AVX__)
		#define CCMATH_HAS_SIMD_AVX 1
	#elif (defined(__SSE__) || (defined(_M_IX86_FP) && _M_IX86_FP >= 1)) && (defined(__SSE2__) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2))
		#define CCMATH_HAS_SIMD_SSE2 1
	#endif

	// If we have any simd instructions, then we define CCMATH_HAS_SIMD so that we can use it in our code.
	#if defined(CCMATH_HAS_SIMD_SSE) || defined(CCMATH_HAS_SIMD_SSE2) || defined(CCMATH_HAS_SIMD_SSE3) || defined(CCMATH_HAS_SIMD_SSSE3) ||                    \
		defined(CCMATH_HAS_SIMD_SSE4_1) || defined(CCMATH_HAS_SIMD_SSE4_2) || defined(CCMATH_HAS_SIMD_AVX) || defined(CCMATH_HAS_SIMD_AVX2) ||                 \
		defined(CCMATH_HAS_SIMD_AVX512BW) || defined(CCMATH_HAS_SIMD_AVX512CD) || defined(CCMATH_HAS_SIMD_AVX512DQ) || defined(CCMATH_HAS_SIMD_AVX512ER) ||    \
		defined(CCMATH_HAS_SIMD_AVX512F) || defined(CCMATH_HAS_SIMD_AVX512PF) || defined(CCMATH_HAS_SIMD_AVX512VL) || defined(CCMATH_HAS_SIMD_NEON)
		#define CCMATH_HAS_SIMD 1
	#else
		#undef CCMATH_HAS_SIMD
	#endif

#endif // CCM_CONFIG_USE_RT_SIMD