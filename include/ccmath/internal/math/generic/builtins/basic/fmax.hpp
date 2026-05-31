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

/// CCMATH_HAS_CONSTEXPR_BUILTIN_FMAX
/// This is a macro that is defined if the compiler has constexpr __builtin functions for fmax that allow static_assert
///
/// Compilers with Support:
/// - GCC 5.1+
/// - Clang 16.0.0+
/// - Intel DPC++ 2023.1.0+

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_FMAX
	#if defined(__GNUC__) && (__GNUC__ > 5 || (__GNUC__ == 5 && __GNUC_MINOR__ >= 1)) && !defined(__clang__) && !defined(__NVCOMPILER_MAJOR__)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_FMAX
	#endif
#endif

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_FMAX
	#if defined(__clang__) && (__clang_major__ > 16 || (__clang_major__ == 16 && __clang_minor__ >= 0)) && !defined(__MSC_VER) &&                              \
		!defined(__INTEL_LLVM_COMPILER)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_FMAX
	#endif
#endif

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_FMAX
	#if defined(__INTEL_LLVM_COMPILER) && (__INTEL_LLVM_COMPILER >= 20230100)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_FMAX
	#endif
#endif

namespace ccm::builtin
{
	// clang-format off
	/**
	 * @internal
	 */
	template <typename T>
	inline constexpr bool has_constexpr_fmax =
		#ifdef CCMATH_HAS_CONSTEXPR_BUILTIN_FMAX
		is_valid_builtin_type<T>;
	#else
        false;
	#endif
	// clang-format on

	/**
	 * @internal
	 * Wrapper for constexpr __builtin_fmax functions.
	 * This should be used internally and always be wrapped in an if constexpr statement.
	 * It exists only to allow for usage of __builtin_fmax functions without triggering a compiler error
	 * when the compiler does not support them.
	 */
	template <typename T>
	constexpr auto fmax(T x, T y) -> std::enable_if_t<has_constexpr_fmax<T>, T>
	{
		if constexpr (std::is_same_v<T, float>) { return __builtin_fmaxf(x, y); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_fmax(x, y); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_fmaxl(x, y); }
		else
		{
			// This should never be reached
			static_assert(ccm::support::always_false<T>, "Unsupported type for __builtin_fmax");
			return T{};
		}
	}
} // namespace ccm::builtin

// Cleanup the global namespace
#undef CCMATH_HAS_CONSTEXPR_BUILTIN_FMAX
