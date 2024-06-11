/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/compiler.hpp"

/// CCMATH_HAS_CONSTEXPR_BUILTIN_FMA
/// This is a macro that is defined if the compiler has constexpr __builtin_copysign that allows static_assert
///
/// Compilers with Support:
/// - GCC 6.1+

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_FMA
	#if defined(CCMATH_COMPILER_GCC) && (CCMATH_COMPILER_GCC_VER > 6 || (CCMATH_COMPILER_GCC_VER == 6 && CCMATH_COMPILER_GCC_PATCH >= 1))
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_FMA
	#endif
#endif
