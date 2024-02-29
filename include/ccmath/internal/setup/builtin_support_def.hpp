/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/setup/compiler_def.hpp"


// This file is used to detect if the compiler has support for certain builtins that are static_assert-able constexpr

#ifndef CCMATH_HAS_BUILTIN_BIT_CAST
	#if (defined(_MSC_VER) && _MSC_VER >= 1927)
		#define CCMATH_HAS_BUILTIN_BIT_CAST
    #endif

#endif

// Some compilers have __builtin_signbit which is constexpr for some compilers
#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_SIGNBIT
	#if !defined(CCMATH_HAS_CONSTEXPR_SIGNBIT)
		// GCC 6.1 has constexpr __builtin_signbit that DOES allow static_assert. Clang does not.
		#if defined(__GNUC__) && (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)
			#define CCMATH_HAS_CONSTEXPR_BUILTIN_SIGNBIT
		#endif
	#endif
#endif

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
	#if !defined(CCMATH_HAS_CONSTEXPR_SIGNBIT)
		// GCC 6.1 has constexpr __builtin_copysign that DOES allow static_assert
		#if defined(__GNUC__) && (__GNUC__ >= 6 && __GNUC_MINOR__ >= 1)
			#define CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
		#endif

		// Clang 5.0.0 has constexpr __builtin_copysign that DOES allow static_assert
		#if defined(__clang__) && (__clang_major__ >= 5)
			#define CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
		#endif
	#endif
#endif



#include "ccmath/internal/setup/compiler_undef.hpp"
