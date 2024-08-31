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

/// CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
/// This is a macro that is defined if the compiler has constexpr __builtin_exp2 that allows static_assert
///
/// Compilers with Support:
/// - GCC 6.1+

#if defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__)
	#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_EXP2
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_EXP2
	#endif
#endif
