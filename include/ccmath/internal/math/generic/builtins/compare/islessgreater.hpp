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

// ReSharper disable once CppUnusedIncludeDirective
#include "ccmath/internal/math/generic/builtins/builtin_helpers.hpp"

#include <type_traits>

/// CCMATH_HAS_CONSTEXPR_BUILTIN_ISLESSGREATER
/// This is a macro that is defined if the compiler has constexpr __builtin_islessgreater that allows static_assert
///
/// Compilers with Support:
/// - GCC 5.1+

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_ISLESSGREATER
	#if defined(__GNUC__) && (__GNUC__ > 5 || (__GNUC__ == 5 && __GNUC_MINOR__ >= 1)) && !defined(__clang__) && !defined(__NVCOMPILER_MAJOR__)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_ISLESSGREATER
	#endif
#endif

/// CCMATH_HAS_BUILTIN_ISLESSGREATER
/// This is a macro that is defined if the compiler has constexpr __builtin functions for islessgreater that allow static_assert
///
/// Compilers with Support:
/// - GCC
/// - Clang

// TODO(IanP): Determine the lowest runtime compiler versions at some point.

#ifndef CCMATH_HAS_BUILTIN_ISLESSGREATER
	#if defined(__GNUC__) || defined(__clang__)
		#define CCMATH_HAS_BUILTIN_ISLESSGREATER
	#endif
#endif

namespace ccm::builtin
{
	// clang-format off
	/**
	 * @internal
	 */
	template <typename T>
	inline constexpr bool has_constexpr_islessgreater =
#ifdef CCMATH_HAS_CONSTEXPR_BUILTIN_ISLESSGREATER
		is_valid_builtin_type<T>;
	#else
			false;
	#endif
	// clang-format on

	// TODO: determine actual compiler/version support for runtime __builtin_islessgreater.
	template <typename T>
	inline constexpr bool has_runtime_islessgreater =
#ifdef CCMATH_HAS_BUILTIN_ISLESSGREATER
		is_valid_builtin_type<T>;
#else
		false;
#endif

	/**
	 * @internal
	 * Wrapper for constexpr __builtin_islessgreater.
	 * This should be used internally and always be wrapped in an if constexpr statement.
	 * It exists only to allow for usage of __builtin_islessgreater without triggering a compiler error
	 * when the compiler does not support it.
	 */
	template <typename T>
	constexpr auto islessgreater_ct(T x, T y) -> std::enable_if_t<has_constexpr_islessgreater<T>, bool>
	{ return __builtin_islessgreater(x, y); }

	template <typename T>
	auto islessgreater_rt(T x, T y) -> std::enable_if_t<has_runtime_islessgreater<T>, bool>
	{ return __builtin_islessgreater(x, y); }
} // namespace ccm::builtin

// Cleanup the global namespace
#undef CCMATH_HAS_CONSTEXPR_BUILTIN_ISLESSGREATER
#undef CCMATH_HAS_BUILTIN_ISLESSGREATER
