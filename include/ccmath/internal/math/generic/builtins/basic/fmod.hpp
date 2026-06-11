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

/// CCMATH_HAS_CONSTEXPR_BUILTIN_FMOD
/// This is a macro that is defined if the compiler has constexpr __builtin functions for fmod that allow static_assert
///
/// Compilers with Support:
/// - GCC 12.1+

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_FMOD
	#if defined(__GNUC__) && (__GNUC__ > 12 || (__GNUC__ == 12 && __GNUC_MINOR__ >= 1)) && !defined(__clang__) && !defined(__NVCOMPILER_MAJOR__)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_FMOD
	#endif
#endif

/// CCMATH_HAS_BUILTIN_FMOD
/// This is a macro that is defined if the compiler has constexpr __builtin functions for fmod that allow static_assert
///
/// Compilers with Support:
/// - GCC
/// - Clang

// TODO(IanP): Determine the lowest runtime compiler versions at some point.

#ifndef CCMATH_HAS_BUILTIN_FMOD
	#if defined(__GNUC__) || defined(__clang__)
		#define CCMATH_HAS_BUILTIN_FMOD
	#endif
#endif

namespace ccm::builtin
{
	// clang-format off
	/**
	 * @internal
	 */
	template <typename T>
	inline constexpr bool has_constexpr_fmod =
		#ifdef CCMATH_HAS_CONSTEXPR_BUILTIN_FMOD
		is_valid_builtin_type<T>;
	#else
			false;
	#endif
	// clang-format on

	template <typename T>
	// TODO: determine actual compiler/version support for runtime __builtin_fmod.
	inline constexpr bool has_runtime_fmod =
#ifdef CCMATH_HAS_BUILTIN_FMOD
		is_valid_builtin_type<T>;
#else
		false;
#endif

	/**
	 * @internal
	 * Wrapper for constexpr __builtin_fmod functions.
	 * This should be used internally and always be wrapped in an if constexpr statement.
	 * It exists only to allow for usage of __builtin_fmod functions without triggering a compiler error
	 * when the compiler does not support them.
	 */
	template <typename T>
	constexpr auto fmod_ct(T x, T y) -> std::enable_if_t<has_constexpr_fmod<T>, T>
	{
		if constexpr (std::is_same_v<T, float>) { return __builtin_fmodf(x, y); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_fmod(x, y); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_fmodl(x, y); }
		else
		{
			// This should never be reached
			static_assert(ccm::support::always_false<T>, "Unsupported type for fmod");
			return T{};
		}
	}

	template <typename T>
	auto fmod_rt(T x, T y) -> std::enable_if_t<has_runtime_fmod<T>, T>
	{
		if constexpr (std::is_same_v<T, float>) { return __builtin_fmodf(x, y); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_fmod(x, y); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_fmodl(x, y); }
		else
		{
			static_assert(ccm::support::always_false<T>, "Unsupported type for fmod");
			return T{};
		}
	}
} // namespace ccm::builtin

// Cleanup the global namespace
#undef CCMATH_HAS_CONSTEXPR_BUILTIN_FMOD
#undef CCMATH_HAS_BUILTIN_FMOD
