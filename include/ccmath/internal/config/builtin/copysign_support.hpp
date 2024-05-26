/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/compiler.hpp"

/// CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
/// This is a macro that is defined if the compiler has constexpr __builtin_copysign that allows static_assert
///
/// Compilers with Support:
/// - GCC 5.1+
/// - Clang 5.0.0+
/// - Clang-CL 5.0.0+ (This is a guess based on Clang support. Needs verification!)
/// - Apple Clang 5.0.0+ (This is a guess based on Clang support. Needs verification!)
/// - DPC++ 2021.1.2+ (Maybe lower? This is as low as I can test currently)
/// - NVIDIA HPC 24.1+

// GCC 5.1+ has constexpr __builtin_copysign that DOES allow static_assert.
#if defined(CCMATH_COMPILER_GCC) && CCMATH_COMPILER_GCC_VER_MAJOR >= 5 && CCMATH_COMPILER_GCC_VER_MINOR >= 1
	#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
	#endif
#endif

// Clang 5.0.0+ has constexpr __builtin_copysign that DOES allow static_assert.
#if defined(CCMATH_COMPILER_CLANG) && CCMATH_COMPILER_CLANG_VER_MAJOR >= 5
    #ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
        #define CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
    #endif
#endif

// Clang-CL 5.0.0+ has constexpr __builtin_copysign that DOES allow static_assert.
// NOTE: This is a guess based on Clang support. Needs verification!
#if defined(CCMATH_COMPILER_CLANG_CL) && CCMATH_COMPILER_CLANG_CL_VER_MAJOR >= 5
	#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
	#endif
#endif

// Apple Clang 5.0.0+ has constexpr __builtin_copysign that DOES allow static_assert.
// TODO: Verify Apple Clang shares functionality with Clang for all builtin support
#if defined(CCMATH_COMPILER_APPLE_CLANG) && CCMATH_COMPILER_APPLE_CLANG_VER_MAJOR >= 5
	#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
	#endif
#endif

// DPC++ 2021.1.2+ has constexpr __builtin_copysign that DOES allow static_assert.
#if defined(CCMATH_COMPILER_INTEL) && CCMATH_COMPILER_INTEL_VER >= 20210102
    #ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
        #define CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
    #endif
#endif

// NVIDIA HPC 24.1+ has constexpr __builtin_copysign that DOES allow static_assert.
#if defined(CCMATH_COMPILER_NVIDIA) && CCMATH_COMPILER_NVIDIA_VER_MAJOR >= 24 && CCMATH_COMPILER_NVIDIA_VER_MINOR >= 1
    #ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
        #define CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
    #endif
#endif
