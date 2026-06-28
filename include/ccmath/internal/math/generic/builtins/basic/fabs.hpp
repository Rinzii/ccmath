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
#include "ccmath/internal/support/always_false.hpp"

#include <type_traits>

/// CCMATH_HAS_CONSTEXPR_BUILTIN_ABS
/// This is a macro that is defined if the compiler has constexpr __builtin functions for abs that allow static_assert
///
/// Compilers with Support:
/// - GCC 5.1+
/// - NVC++ 22.7+ (Lowest tested version)
/// - Clang 5.0.0+
/// - Intel DPC++ 2021.1.2+ (Lowest tested version)

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_ABS
	#if defined(__GNUC__) && (__GNUC__ > 5 || (__GNUC__ == 5 && __GNUC_MINOR__ >= 1)) && !defined(__clang__) && !defined(__NVCOMPILER_MAJOR__)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_ABS
	#endif
#endif

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_ABS
	#if defined(__NVCOMPILER_MAJOR__) && (__NVCOMPILER_MAJOR__ > 22 || (__NVCOMPILER_MAJOR__ == 22 && __NVCOMPILER_MINOR__ >= 7))
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_ABS
	#endif
#endif

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_ABS
	#if defined(__clang__) && (__clang_major__ > 5 || (__clang_major__ == 5 && __clang_minor__ >= 0)) && !defined(_MSC_VER) && !defined(__INTEL_LLVM_COMPILER)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_ABS
	#endif
#endif

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_ABS
	#if defined(__INTEL_LLVM_COMPILER) && (__INTEL_LLVM_COMPILER >= 202110)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_ABS
	#endif
#endif

/// CCMATH_HAS_BUILTIN_ABS
/// This is a macro that is defined if the compiler has constexpr __builtin functions for abs that allow static_assert
///
/// Compilers with Support:
/// - GCC
/// - Clang

// TODO(IanP): Determine the lowest runtime compiler versions at some point.

#ifndef CCMATH_HAS_BUILTIN_ABS
	#if defined(__GNUC__) || defined(__clang__)
		#define CCMATH_HAS_BUILTIN_ABS
	#endif
#endif

namespace ccm::builtin
{
	// clang-format off
	/**
	 * @internal
	 */
	template <typename T>
	inline constexpr bool has_constexpr_abs =
		#ifdef CCMATH_HAS_CONSTEXPR_BUILTIN_ABS
		is_valid_builtin_type<T>;
	#else
        false;
	#endif
	// clang-format on

	// TODO: determine actual compiler/version support for runtime __builtin_fabs.
	template <typename T>
	inline constexpr bool has_runtime_abs =
#ifdef CCMATH_HAS_BUILTIN_ABS
		is_valid_builtin_type<T>;
#else
		false;
#endif

	/**
	 * @internal
	 * Wrapper for constexpr __builtin_fabs functions.
	 * This should be used internally and always be wrapped in an if constexpr statement.
	 * It exists only to allow for usage of __builtin_fabs functions without triggering a compiler error
	 * when the compiler does not support them.
	 */
	template <typename T> constexpr auto abs_ct(T x) -> std::enable_if_t<has_constexpr_abs<T>, T>
	{
		if constexpr (std::is_same_v<T, float>)
		{
			return __builtin_fabsf(x);
		} else if constexpr (std::is_same_v<T, double>)
		{
			return __builtin_fabs(x);
		} else if constexpr (std::is_same_v<T, long double>)
		{
			return __builtin_fabsl(x);
		} else
		{
			// This should never be reached
			static_assert(support::always_false<T>, "Unsupported type for abs");
			return T{};
		}
	}

	template <typename T> auto abs_rt(T x) -> std::enable_if_t<has_runtime_abs<T>, T>
	{
		if constexpr (std::is_same_v<T, float>)
		{
			return __builtin_fabsf(x);
		} else if constexpr (std::is_same_v<T, double>)
		{
			return __builtin_fabs(x);
		} else if constexpr (std::is_same_v<T, long double>)
		{
			return __builtin_fabsl(x);
		} else
		{
			// This should never be reached
			static_assert(support::always_false<T>, "Unsupported type for abs");
			return T{};
		}
	}
} // namespace ccm::builtin

// Cleanup the global namespace
#undef CCMATH_HAS_CONSTEXPR_BUILTIN_ABS
#undef CCMATH_HAS_BUILTIN_ABS
