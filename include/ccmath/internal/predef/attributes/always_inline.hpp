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

// This macro is used to force inlining of a function.
// Should only be used for runtime func and not used with compile time func.
// If you want inlining for a compile time function, use the inline keyword.
#ifndef CCM_ALWAYS_INLINE
	#if defined(CCM_CONFIG_NO_FORCED_INLINE)
		#define CCM_ALWAYS_INLINE inline
	#elif defined(__GNUC__) || (defined(__clang__) && !defined(_MSC_VER))
		#define CCM_ALWAYS_INLINE __attribute__((always_inline)) inline
	#elif defined(_MSC_VER)
		#define CCM_ALWAYS_INLINE __forceinline
	#else
		#define CCM_ALWAYS_INLINE inline
	#endif
#endif

// Keep this separate for CUDA and HIP to be explicit.
#if defined(CCM_CONFIG_NO_FORCED_INLINE)
	#define CCM_CUDA_ALWAYS_INLINE inline
#elif defined(__CUDACC__)
	#define CCM_CUDA_ALWAYS_INLINE __forceinline__
#else
	#define CCM_CUDA_ALWAYS_INLINE inline
#endif

#if defined(CCM_CONFIG_NO_FORCED_INLINE)
	#define CCM_HIP_ALWAYS_INLINE inline
#elif defined(__HIPCC__)
	#define CCM_HIP_ALWAYS_INLINE __forceinline__
#else
	#define CCM_HIP_ALWAYS_INLINE inline
#endif
