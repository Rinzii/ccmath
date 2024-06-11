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
/// - Apple Clang 9.0.0+ (This is a guess based on Clang support. Needs verification!)
/// - Clang-CL 9.0.0+ (This is a guess based on Clang support. Needs verification!)
/// - DPC++ 2021.1.2+ (Maybe lower? This is as low as I can test currently)
/// - NVIDIA HPC 22.7+ (Maybe lower? This is as low as I can test currently)
/// - MSVC 19.27+

// GCC 11.1+ has __builtin_bit_cast
#if defined(__GNUC__) && (__GNUC__ > 11 || (__GNUC__ == 11 && __GNUC_MINOR__ >= 1)) && !defined(__clang__) && !defined(__NVCOMPILER) && !defined(__NVCOMPILER_LLVM__) && !defined(__CUDACC__)
    #ifndef CCMATH_HAS_BUILTIN_BIT_CAST
        #define CCMATH_HAS_BUILTIN_BIT_CAST
    #endif
#endif

// Clang 9.0.0+ has __builtin_bit_cast
#if defined(__clang__) && !defined(__apple_build_version__)  && __clang_major__ >= 9
    #ifndef CCMATH_HAS_BUILTIN_BIT_CAST
        #define CCMATH_HAS_BUILTIN_BIT_CAST
    #endif
#endif

// Apple Clang 9.0.0+ has __builtin_bit_cast
// TODO: Verify Apple Clang shares functionality with Clang for all builtin support
#if defined(__clang__) && defined(__apple_build_version__) && (__clang_major__ >= 9)
	#ifndef CCMATH_HAS_BUILTIN_BIT_CAST
		#define CCMATH_HAS_BUILTIN_BIT_CAST
	#endif
#endif

// Clang-CL 9.0.0+ has __builtin_bit_cast
// TODO: This is a guess based on Clang support. Needs verification!
#if defined(__clang__) && defined(_MSC_VER) && (__clang_major__ >= 9)
	#ifndef CCMATH_HAS_BUILTIN_BIT_CAST
		#define CCMATH_HAS_BUILTIN_BIT_CAST
	#endif
#endif

// DPC++ 2021.1.2+ has __builtin_bit_cast
#if (defined(SYCL_LANGUAGE_VERSION) || defined(__INTEL_LLVM_COMPILER))  && (__INTEL_LLVM_COMPILER >= 20210102)
    #ifndef CCMATH_HAS_BUILTIN_BIT_CAST
        #define CCMATH_HAS_BUILTIN_BIT_CAST
    #endif
#endif

// NVIDIA HPC 22.7+ has __builtin_bit_cast (Maybe lower? This is as low as I can test currently)
#if (defined(__NVCOMPILER) || defined(__NVCOMPILER_LLVM__)) && (__NVCOMPILER_MAJOR__ >= 22 && __NVCOMPILER_MINOR__ >= 7)
	#ifndef CCMATH_HAS_BUILTIN_BIT_CAST
		#define CCMATH_HAS_BUILTIN_BIT_CAST
	#endif
#endif

// MSVC 19.27+ has __builtin_bit_cast
#if defined(_MSC_VER) && !defined(__clang__) && (_MSC_VER >= 1927)
    #ifndef CCMATH_HAS_BUILTIN_BIT_CAST
        #define CCMATH_HAS_BUILTIN_BIT_CAST
    #endif
#endif
