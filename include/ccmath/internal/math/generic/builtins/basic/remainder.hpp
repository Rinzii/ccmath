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

/// CCMATH_HAS_CONSTEXPR_BUILTIN_REMAINDER
/// This is a macro that is defined if the compiler has constexpr __builtin functions for remainder that allow static_assert
///
/// Compilers with Support:
/// - GCC 5.1+

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_REMAINDER
	#if defined(__GNUC__) && (__GNUC__ > 5 || (__GNUC__ == 5 && __GNUC_MINOR__ >= 1)) && !defined(__clang__) && !defined(__NVCOMPILER_MAJOR__)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_REMAINDER
	#endif
#endif

/// CCMATH_HAS_BUILTIN_REMAINDER
/// This is a macro that is defined if the compiler has constexpr __builtin functions for remainder that allow static_assert
///
/// Compilers with Support:
/// - GCC
/// - Clang

// TODO(IanP): Determine the lowest runtime compiler versions at some point.

#ifndef CCMATH_HAS_BUILTIN_REMAINDER
	#if defined(__GNUC__) || defined(__clang__)
		#define CCMATH_HAS_BUILTIN_REMAINDER
	#endif
#endif

namespace ccm::builtin
{
	// clang-format off
    /**
     * @internal
     */
    template <typename T>
    inline constexpr bool has_constexpr_remainder =
#ifdef CCMATH_HAS_CONSTEXPR_BUILTIN_REMAINDER
        is_valid_builtin_type<T>;
#else
        false;
#endif
	// clang-format on

	template <typename T>
	// TODO: determine actual compiler/version support for runtime __builtin_remainder.
	inline constexpr bool has_runtime_remainder =
#ifdef CCMATH_HAS_BUILTIN_REMAINDER
		is_valid_builtin_type<T>;
#else
		false;
#endif

	/**
	 * @internal
	 * Wrapper for constexpr __builtin_remainder functions.
	 * This should be used internally and always be wrapped in an if constexpr statement.
	 * It exists only to allow for usage of __builtin_remainder functions without triggering a compiler error
	 * when the compiler does not support them.
	 */
	template <typename T> constexpr auto remainder_ct(T x, T y) -> std::enable_if_t<has_constexpr_remainder<T>, T>
	{
		if constexpr (std::is_same_v<T, float>)
		{
			return __builtin_remainderf(x, y);
		} else if constexpr (std::is_same_v<T, double>)
		{
			return __builtin_remainder(x, y);
		} else if constexpr (std::is_same_v<T, long double>)
		{
			return __builtin_remainderl(x, y);
		} else
		{
			// This should never be reached
			static_assert(ccm::support::always_false<T>, "Unsupported type for remainder");
			return T{};
		}
	}

	template <typename T> auto remainder_rt(T x, T y) -> std::enable_if_t<has_runtime_remainder<T>, T>
	{
		if constexpr (std::is_same_v<T, float>)
		{
			return __builtin_remainderf(x, y);
		} else if constexpr (std::is_same_v<T, double>)
		{
			return __builtin_remainder(x, y);
		} else if constexpr (std::is_same_v<T, long double>)
		{
			return __builtin_remainderl(x, y);
		} else
		{
			static_assert(ccm::support::always_false<T>, "Unsupported type for remainder");
			return T{};
		}
	}
} // namespace ccm::builtin

// Cleanup the global namespace
#undef CCMATH_HAS_CONSTEXPR_BUILTIN_REMAINDER
#undef CCMATH_HAS_BUILTIN_REMAINDER
