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

#include <type_traits>

#include "ccmath/internal/math/generic/builtins/builtin_helpers.hpp"

/// CCMATH_HAS_CONSTEXPR_BUILTIN_FMA
/// This is a macro that is defined if the compiler has constexpr __builtin_copysign that allows static_assert
///
/// Compilers with Support:
/// - GCC 5.1+

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_FMA
#if defined(__GNUC__) && (__GNUC__ > 5 || (__GNUC__ == 5 && __GNUC_MINOR__ >= 1)) && !defined(__clang__) && !defined(__NVCOMPILER_MAJOR__)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_FMA
#endif
#endif

namespace ccm::builtin
{
	// clang-format off
	template <typename T>
	inline constexpr bool has_constexpr_fma =
		#ifdef CCMATH_HAS_CONSTEXPR_BUILTIN_FMA
			is_valid_builtin_type<T>;
		#else
			false;
		#endif
	// clang-format on

	/**
	 * Wrapper for constexpr __builtin_fma.
	 * This should be used internally and always be wrapped in an if constexpr statement.
	 * It exists only to allow for usage of __builtin_fma without triggering a compiler error
	 * when the compiler does not support it.
	 * This is thanks to taking advantage of ADL.
	 */
	template <typename T>
	constexpr auto fma(T x, T y, T z)
		-> std::enable_if_t<has_constexpr_fma<T>, T>
	{
		if constexpr (std::is_same_v<T, float>)
		{
			return __builtin_fmaf(x, y, z);
		}
		if constexpr (std::is_same_v<T, double>)
		{
			return __builtin_fma(x, y, z);
		}
		if constexpr (std::is_same_v<T, long double>)
		{
			return __builtin_fmal(x, y, z);
		}
		// This should never be reached
		return T{};
	}
} // namespace ccm::builtin

// Cleanup the global namespace
#undef CCMATH_HAS_CONSTEXPR_BUILTIN_FMA
