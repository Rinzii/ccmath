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
