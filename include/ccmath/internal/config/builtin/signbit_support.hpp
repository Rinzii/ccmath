/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/compiler.hpp"

/// CCMATH_HAS_CONSTEXPR_BUILTIN_SIGNBIT
/// This is a macro that is defined if the compiler has constexpr __builtin_signbit that allows static_assert
///
/// Compilers with Support:
/// - GCC 6.1+
/// - NVIDIA HPC 24.1+

// GCC 6.1 has constexpr __builtin_signbit that DOES allow static_assert.
#if defined(CCMATH_COMPILER_GCC) && CCMATH_COMPILER_GCC_VER_MAJOR >= 6 && CCMATH_COMPILER_GCC_VER_MINOR >= 1
	#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_SIGNBIT
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_SIGNBIT
	#endif
#endif

// NVIDIA HPC 24.1+ has constexpr __builtin_signbit that DOES allow static_assert
#if defined(CCMATH_COMPILER_NVIDIA) && CCMATH_COMPILER_NVIDIA_VER_MAJOR >= 24 && CCMATH_COMPILER_NVIDIA_VER_MINOR >= 1
	#ifndef CCMATH_HAS_BUILTIN_BIT_CAST
		#define CCMATH_HAS_BUILTIN_BIT_CAST
	#endif
#endif
