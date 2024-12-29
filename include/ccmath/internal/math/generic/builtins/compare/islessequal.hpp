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

/// CCMATH_HAS_CONSTEXPR_BUILTIN_ISLESSEQUAL
/// This is a macro that is defined if the compiler has constexpr __builtin_islessequal that allows static_assert
///
/// Compilers with Support:
/// - GCC 5.1+

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_ISLESSEQUAL
#if defined(__GNUC__) && (__GNUC__ > 5 || (__GNUC__ == 5 && __GNUC_MINOR__ >= 1)) && !defined(__clang__) && !defined(__NVCOMPILER_MAJOR__)
#define CCMATH_HAS_CONSTEXPR_BUILTIN_ISLESSEQUAL
#endif
#endif

namespace ccm::builtin
{
	// clang-format off
	template <typename T>
	inline constexpr bool has_constexpr_islessequal =
#ifdef CCMATH_HAS_CONSTEXPR_BUILTIN_ISLESSEQUAL
		is_valid_builtin_type<T>;
	#else
			false;
	#endif
	// clang-format on

	/**
	 * Wrapper for constexpr __builtin_islessequal.
	 * This should be used internally and always be wrapped in an if constexpr statement.
	 * It exists only to allow for usage of __builtin_islessequal without triggering a compiler error
	 * when the compiler does not support it.
	 */
	template <typename T>
	constexpr auto islessequal(T x, T y) -> std::enable_if_t<has_constexpr_islessequal<T>, bool>
	{
		return __builtin_islessequal(x, y);
	}
} // namespace ccm::builtin

// Cleanup the global namespace
#undef CCMATH_HAS_CONSTEXPR_BUILTIN_ISLESSEQUAL