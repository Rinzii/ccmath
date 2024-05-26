/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

/// CCMATH_HAS_BUILTIN_BIT_CAST
/// This is a macro that is defined if the compiler has __builtin_bit_cast
///
/// Compilers with Support:
/// - GCC 11.1+
/// - Clang 9.0.0+
/// - Clang-CL 9.0.0+ (This is a guess based on Clang support. Needs verification!)
/// - Apple Clang 9.0.0+ (This is a guess based on Clang support. Needs verification!)
/// - DPC++ 2021.1.2+ (Maybe lower? This is as low as I can test currently)
/// - NVIDIA HPC 22.7+ (Maybe lower? This is as low as I can test currently)
/// - MSVC 19.27+

// GCC 11.1+ has __builtin_bit_cast
#if defined(CCMATH_COMPILER_GCC) && CCMATH_COMPILER_GCC_VER_MAJOR >= 11 && CCMATH_COMPILER_GCC_VER_MINOR >= 1
    #ifndef CCMATH_HAS_BUILTIN_BIT_CAST
        #define CCMATH_HAS_BUILTIN_BIT_CAST
    #endif
#endif

// Clang 9.0.0+ has __builtin_bit_cast
#if defined(CCMATH_COMPILER_CLANG) && CCMATH_COMPILER_CLANG_VER_MAJOR >= 9
    #ifndef CCMATH_HAS_BUILTIN_BIT_CAST
        #define CCMATH_HAS_BUILTIN_BIT_CAST
    #endif
#endif

// Clang-CL 9.0.0+ has __builtin_bit_cast
// NOTE: This is a guess based on Clang support. Needs verification!
#if defined(CCMATH_COMPILER_CLANG_CL) && CCMATH_COMPILER_CLANG_CL_VER_MAJOR >= 9
	#ifndef CCMATH_HAS_BUILTIN_BIT_CAST
		#define CCMATH_HAS_BUILTIN_BIT_CAST
	#endif
#endif

// Apple Clang 9.0.0+ has __builtin_bit_cast
// TODO: Verify Apple Clang shares functionality with Clang for all builtin support
#if defined(CCMATH_COMPILER_APPLE_CLANG) && CCMATH_COMPILER_APPLE_CLANG_VER_MAJOR >= 9
	#ifndef CCMATH_HAS_BUILTIN_BIT_CAST
		#define CCMATH_HAS_BUILTIN_BIT_CAST
	#endif
#endif

// DPC++ 2021.1.2+ has __builtin_bit_cast
#if defined(CCMATH_COMPILER_INTEL) && CCMATH_COMPILER_INTEL_VER >= 20210102
    #ifndef CCMATH_HAS_BUILTIN_BIT_CAST
        #define CCMATH_HAS_BUILTIN_BIT_CAST
    #endif
#endif

// NVIDIA HPC 22.7+ has __builtin_bit_cast (Maybe lower? This is as low as I can test currently)
#if defined(CCMATH_COMPILER_NVIDIA) && CCMATH_COMPILER_NVIDIA_VER_MAJOR >= 22 && CCMATH_COMPILER_NVIDIA_VER_MINOR >= 7
	#ifndef CCMATH_HAS_BUILTIN_BIT_CAST
		#define CCMATH_HAS_BUILTIN_BIT_CAST
	#endif
#endif

// MSVC 19.27+ has __builtin_bit_cast
#if (defined(CCMATH_COMPILER_MSVC) && CCMATH_COMPILER_MSVC_VER >= 1927)
    #ifndef CCMATH_HAS_BUILTIN_BIT_CAST
        #define CCMATH_HAS_BUILTIN_BIT_CAST
    #endif
#endif
