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

/// CCMATH_HAS_CONSTEXPR_BUILTIN_GAMMA
/// This is a macro that is defined if the compiler has constexpr __builtin functions for tgamma that allow static_assert
///
/// Compilers with Support:
/// - GCC 5.1+

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_GAMMA
	#if defined(__GNUC__) && (__GNUC__ > 5 || (__GNUC__ == 5 && __GNUC_MINOR__ >= 1)) && !defined(__clang__) && !defined(__NVCOMPILER_MAJOR__)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_GAMMA
	#endif
#endif

/// CCMATH_HAS_BUILTIN_GAMMA
/// This is a macro that is defined if the compiler has constexpr __builtin functions for gamma that allow static_assert
///
/// Compilers with Support:
/// - GCC
/// - Clang

// TODO(IanP): Determine the lowest runtime compiler versions at some point.

#ifndef CCMATH_HAS_BUILTIN_GAMMA
	#if defined(__GNUC__) || defined(__clang__)
		#define CCMATH_HAS_BUILTIN_GAMMA
	#endif
#endif

namespace ccm::builtin
{
	// clang-format off
	/**
	 * @internal
	 */
	template <typename T>
	inline constexpr bool has_constexpr_gamma =
#ifdef CCMATH_HAS_CONSTEXPR_BUILTIN_GAMMA
		is_valid_transcendental_builtin_type<T>;
	#else
			false;
	#endif
	// clang-format on

	template <typename T>
	// TODO: determine actual compiler/version support for runtime __builtin_gamma.
	inline constexpr bool has_runtime_gamma =
#ifdef CCMATH_HAS_BUILTIN_GAMMA
		is_valid_transcendental_builtin_type<T>;
#else
		false;
#endif

	/**
	 * @internal
	 * Wrapper for constexpr __builtin_tgamma functions.
	 * This should be used internally and always be wrapped in an if constexpr statement.
	 * It exists only to allow for usage of __builtin_tgamma functions without triggering a compiler error
	 * when the compiler does not support them.
	 */
	template <typename T> constexpr auto gamma_ct(T x) -> std::enable_if_t<has_constexpr_gamma<T>, T>
	{
		if constexpr (std::is_same_v<T, float>)
		{
			return __builtin_tgammaf(x);
		} else if constexpr (std::is_same_v<T, double>)
		{
			return __builtin_tgamma(x);
		} else if constexpr (std::is_same_v<T, long double>)
		{
			return __builtin_tgammal(x);
		} else
		{
			static_assert(ccm::support::always_false<T>, "Unsupported type for gamma");
			return T{};
		}
	}

	template <typename T> auto gamma_rt(T x) -> std::enable_if_t<has_runtime_gamma<T>, T>
	{
		if constexpr (std::is_same_v<T, float>)
		{
			return __builtin_tgammaf(x);
		} else if constexpr (std::is_same_v<T, double>)
		{
			return __builtin_tgamma(x);
		} else if constexpr (std::is_same_v<T, long double>)
		{
			return __builtin_tgammal(x);
		} else
		{
			static_assert(ccm::support::always_false<T>, "Unsupported type for gamma");
			return T{};
		}
	}
} // namespace ccm::builtin

// Cleanup the global namespace

#undef CCMATH_HAS_CONSTEXPR_BUILTIN_GAMMA
#undef CCMATH_HAS_BUILTIN_GAMMA
