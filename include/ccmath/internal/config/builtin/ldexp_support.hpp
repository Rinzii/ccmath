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

/// CCMATH_HAS_CONSTEXPR_BUILTIN_LDEXP
/// This is a macro that is defined if the compiler has constexpr __builtin_ldexp that allows static_assert
///
/// Compilers with Support:
/// - GCC 5.1+

#if defined(__GNUC__) && (__GNUC__ > 5 || (__GNUC__ == 5 && __GNUC_MINOR__ >= 1)) && !defined(__clang__)
	#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_LDEXP
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_LDEXP
	#endif
#endif
