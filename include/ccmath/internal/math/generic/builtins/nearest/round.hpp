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

/// CCMATH_HAS_BUILTIN_ROUND
/// This is a macro that is defined if the compiler has constexpr __builtin functions for round that allow static_assert
///
/// Compilers with Support:
/// - GCC
/// - Clang

// TODO(IanP): Determine the lowest runtime compiler versions at some point.

#ifndef CCMATH_HAS_BUILTIN_ROUND
	#if defined(__GNUC__) || defined(__clang__)
		#define CCMATH_HAS_BUILTIN_ROUND
	#endif
#endif

/// CCMATH_HAS_BUILTIN_LROUND
/// This is a macro that is defined if the compiler has constexpr __builtin functions for lround that allow static_assert
///
/// Compilers with Support:
/// - GCC
/// - Clang

// TODO(IanP): Determine the lowest runtime compiler versions at some point.

#ifndef CCMATH_HAS_BUILTIN_LROUND
	#if defined(__GNUC__) || defined(__clang__)
		#define CCMATH_HAS_BUILTIN_LROUND
	#endif
#endif

namespace ccm::builtin
{
	// clang-format off
	/**
	 * @internal
	 */
	template <typename T>
	inline constexpr bool has_constexpr_round =
		#ifdef CCMATH_HAS_CONSTEXPR_BUILTIN_ROUND
			is_valid_builtin_type<T>;
		#else
			false;
		#endif

	/**
	 * @internal
	 */
	template <typename T>
	inline constexpr bool has_constexpr_lround =
		#ifdef CCMATH_HAS_CONSTEXPR_BUILTIN_LROUND
			is_valid_builtin_type<T>;
		#else
			false;
		#endif
	// clang-format on

	// TODO: determine actual compiler/version support for runtime __builtin_round.
	template <typename T>
	inline constexpr bool has_runtime_round =
#ifdef CCMATH_HAS_BUILTIN_ROUND
		is_valid_builtin_type<T>;
#else
		false;
#endif

	// TODO: determine actual compiler/version support for runtime __builtin_lround.
	template <typename T>
	inline constexpr bool has_runtime_lround =
#ifdef CCMATH_HAS_BUILTIN_LROUND
		is_valid_builtin_type<T>;
#else
		false;
#endif

	/**
	 * @internal
	 * Wrapper for constexpr __builtin_round functions.
	 * This should be used internally and always be wrapped in an if constexpr statement.
	 * It exists only to allow for usage of __builtin_round functions without triggering a compiler error
	 * when the compiler does not support them.
	 */
	template <typename T>
	constexpr auto round_ct(T x) -> std::enable_if_t<has_constexpr_round<T>, T>
	{
		if constexpr (std::is_same_v<T, float>) { return __builtin_roundf(x); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_round(x); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_roundl(x); }
		else
		{
			// This should never be reached
			static_assert(ccm::support::always_false<T>, "Unsupported type for round");
			return T{};
		}
	}

	/**
	 * @internal
	 * Wrapper for constexpr __builtin_lround functions.
	 * This should be used internally and always be wrapped in an if constexpr statement.
	 * It exists only to allow for usage of __builtin_lround functions without triggering a compiler error
	 * when the compiler does not support them.
	 */
	template <typename T>
	constexpr auto lround_ct(T x) -> std::enable_if_t<has_constexpr_lround<T>, T>
	{
		if constexpr (std::is_same_v<T, float>) { return __builtin_lroundf(x); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_lround(x); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_lroundl(x); }
		else
		{
			// This should never be reached
			static_assert(ccm::support::always_false<T>, "Unsupported type for lround");
			return T{};
		}
	}

	template <typename T>
	auto round_rt(T x) -> std::enable_if_t<has_runtime_round<T>, T>
	{
		if constexpr (std::is_same_v<T, float>) { return __builtin_roundf(x); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_round(x); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_roundl(x); }
		else
		{
			static_assert(ccm::support::always_false<T>, "Unsupported type for round");
			return T{};
		}
	}

	template <typename T>
	auto lround_rt(T x) -> std::enable_if_t<has_runtime_lround<T>, T>
	{
		if constexpr (std::is_same_v<T, float>) { return __builtin_lroundf(x); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_lround(x); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_lroundl(x); }
		else
		{
			// This should never be reached
			static_assert(ccm::support::always_false<T>, "Unsupported type for lround");
			return T{};
		}
	}
} // namespace ccm::builtin

// Cleanup the global namespace
#undef CCMATH_HAS_CONSTEXPR_BUILTIN_ROUND
#undef CCMATH_HAS_CONSTEXPR_BUILTIN_LROUND
#undef CCMATH_HAS_BUILTIN_LROUND
#undef CCMATH_HAS_BUILTIN_ROUND
