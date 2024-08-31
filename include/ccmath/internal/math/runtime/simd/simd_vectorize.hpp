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

#ifndef CCM_SIMD_VECTORIZE
	#if defined(_OPENMP)
		#define CCM_SIMD_VECTORIZE _Pragma("omp simd")
// msvc
	#elif defined(_MSC_VER) && !defined(__clang__)
		#define CCM_SIMD_VECTORIZE __pragma(loop(ivdep))
	#elif defined(__clang__)
		#define CCM_SIMD_VECTORIZE _Pragma("clang loop vectorize(enable)")
	#elif defined(__GNUC__) && !defined(__FUJITSU)
		#define CCM_SIMD_VECTORIZE _Pragma("GCC ivdep")
	#else
		#define CCM_SIMD_VECTORIZE
	#endif
#endif
