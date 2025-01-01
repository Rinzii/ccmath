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

#include "ccmath/internal/math/generic/builtins/builtin_helpers.hpp"

#include <type_traits>

/// CCMATH_HAS_CONSTEXPR_BUILTIN_ROUND
/// This is a macro that is defined if the compiler has constexpr __builtin functions for round that allow static_assert
///
/// Compilers with Support:
/// - GCC 5.1+

/// CCMATH_HAS_CONSTEXPR_BUILTIN_LROUND
/// This is a macro that is defined if the compiler has constexpr __builtin functions for lround that allow static_assert
///
/// Compilers with Support:
/// - GCC 5.1+

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_ROUND
	#if defined(__GNUC__) && (__GNUC__ > 5 || (__GNUC__ == 5 && __GNUC_MINOR__ >= 1)) && !defined(__clang__) && !defined(__NVCOMPILER_MAJOR__)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_ROUND
	#endif
#endif

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_LROUND
	#if defined(__GNUC__) && (__GNUC__ > 5 || (__GNUC__ == 5 && __GNUC_MINOR__ >= 1)) && !defined(__clang__) && !defined(__NVCOMPILER_MAJOR__)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_LROUND
	#endif
#endif

namespace ccm::builtin
{
	// clang-format off
	template <typename T>
	inline constexpr bool has_constexpr_round =
		#ifdef CCMATH_HAS_CONSTEXPR_BUILTIN_ROUND
			is_valid_builtin_type<T>;
		#else
			false;
		#endif

	template <typename T>
	inline constexpr bool has_constexpr_lround =
		#ifdef CCMATH_HAS_CONSTEXPR_BUILTIN_LROUND
			is_valid_builtin_type<T>;
		#else
			false;
		#endif
	// clang-format on

	/**
	 * Wrapper for constexpr __builtin_round functions.
	 * This should be used internally and always be wrapped in an if constexpr statement.
	 * It exists only to allow for usage of __builtin_round functions without triggering a compiler error
	 * when the compiler does not support them.
	 */
	template <typename T>
	constexpr auto round(T x) -> std::enable_if_t<has_constexpr_round<T>, T>
	{
		if constexpr (std::is_same_v<T, float>) { return __builtin_roundf(x); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_round(x); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_roundl(x); }
		// This should never be reached
		return T{};
	}

	/**
	 * Wrapper for constexpr __builtin_lround functions.
	 * This should be used internally and always be wrapped in an if constexpr statement.
	 * It exists only to allow for usage of __builtin_lround functions without triggering a compiler error
	 * when the compiler does not support them.
	 */
	template <typename T>
	constexpr auto lround(T x) -> std::enable_if_t<has_constexpr_lround<T>, T>
	{
		if constexpr (std::is_same_v<T, float>) { return __builtin_lroundf(x); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_lround(x); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_lroundl(x); }
		// This should never be reached
		return T{};
	}
} // namespace ccm::builtin

// Cleanup the global namespace
#undef CCMATH_HAS_CONSTEXPR_BUILTIN_ROUND
#undef CCMATH_HAS_CONSTEXPR_BUILTIN_LROUND
