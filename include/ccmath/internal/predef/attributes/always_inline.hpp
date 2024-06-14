/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

// This macro is used to force inlining of a function.
// Should only be used for runtime functions and not used with compile time functions.
// If you want inlining for a compile time function, use the inline keyword.
#ifndef CCM_ALWAYS_INLINE
	#if defined(__GNUC__) || defined(__clang__)
		#define CCM_ALWAYS_INLINE __attribute__((always_inline))
	#elif defined(_MSC_VER)
		#define CCM_ALWAYS_INLINE __forceinline
	#else
		#define CCM_ALWAYS_INLINE
	#endif
#endif

// Keep this separate for CUDA and HIP to be explicit.
#if defined(__CUDACC__)
	#define CCM_CUDA_ALWAYS_INLINE __forceinline__
#endif

#if defined(__HIPCC__)
	#define CCM_HIP_ALWAYS_INLINE __forceinline__
#endif