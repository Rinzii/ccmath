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

/// CCMATH_HAS_CONSTEXPR_BUILTIN_ISNAN
/// This is a macro that is defined if the compiler has constexpr __builtin functions for isnan that allow static_assert
///
/// Compilers with Support:
/// - GCC 5.1+
/// - Clang 5.0.0+
/// NVC++ 22.7+ (Lowest tested version)
/// Intel DPC++ 2021.1.2+ (Lowest tested version)

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_ISNAN
	#if defined(__GNUC__) && (__GNUC__ > 5 || (__GNUC__ == 5 && __GNUC_MINOR__ >= 1)) && !defined(__clang__) && !defined(__NVCOMPILER_MAJOR__)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_ISNAN
	#endif
#endif

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_ISNAN
	#if defined(__clang__) && (__clang_major__ > 5 || (__clang_major__ == 5 && __clang_minor__ >= 0)) && !defined(__MSC_VER) && !defined(__INTEL_LLVM_COMPILER)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_ISNAN
	#endif
#endif

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_ISNAN
	#if defined(__NVCOMPILER_MAJOR__) && (__NVCOMPILER_MAJOR__ > 22 || (__NVCOMPILER_MAJOR__ == 22 && __NVCOMPILER_MINOR__ >= 7))
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_ISNAN
	#endif
#endif

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_ISNAN
	#if defined(__INTEL_LLVM_COMPILER) && (__INTEL_LLVM_COMPILER >= 202110)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_ISNAN
	#endif
#endif

namespace ccm::builtin
{
	// clang-format off
	template <typename T>
	inline constexpr bool has_constexpr_isnan =
#ifdef CCMATH_HAS_CONSTEXPR_BUILTIN_ISNAN
		is_valid_builtin_type<T>;
	#else
			false;
	#endif
	// clang-format on

	/**
	 * Wrapper for constexpr __builtin isnan functions.
	 * This should be used internally and always be wrapped in an if constexpr statement.
	 * It exists only to allow for usage of __builtin isnan functions without triggering a compiler error
	 * when the compiler does not support them.
	 */
	template <typename T>
	constexpr auto isnan(T x) -> std::enable_if_t<has_constexpr_isnan<T>, bool>
	{
		if constexpr (std::is_same_v<T, float>) { return __builtin_isnan(x); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_isnan(x); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_isnan(x); }
		// This should never be reached
		return false;
	}
} // namespace ccm::builtin

// Cleanup the global namespace
#undef CCMATH_HAS_CONSTEXPR_BUILTIN_ISNAN
