/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
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