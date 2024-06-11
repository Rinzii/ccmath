/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
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