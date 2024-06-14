/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#ifndef CCM_SIMD_VECTORIZE
	#if defined(_OPENMP)
		#define CCM_SIMD_VECTORIZE _Pragma("omp simd")
	#elif defined(__clang__)
		#define CCM_SIMD_VECTORIZE _Pragma("clang loop vectorize(enable)")
	#elif defined(__GNUC__) && !defined(__FUJITSU)
		#define CCM_SIMD_VECTORIZE _Pragma("GCC ivdep")
	#else
		#define CCM_SIMD_VECTORIZE
	#endif
#endif