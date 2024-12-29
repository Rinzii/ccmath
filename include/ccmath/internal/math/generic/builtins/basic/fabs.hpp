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

/// CCMATH_HAS_CONSTEXPR_BUILTIN_ABS
/// This is a macro that is defined if the compiler has constexpr __builtin functions for abs that allow static_assert
///
/// Compilers with Support:
/// - GCC 5.1+
/// - NVC++ 22.7+ (Lowest tested version)
/// - Clang trunk allows it, but it is not yet in a release
/// TODO: Add clang version when it is released

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

namespace ccm::builtin
{
	// clang-format off
	template <typename T>
	inline constexpr bool has_constexpr_abs =
		#ifdef CCMATH_HAS_CONSTEXPR_BUILTIN_ABS
		std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, long double> || std::is_same_v<T, long long> || (
			std::is_integral_v<T> && std::is_signed_v<T>) || (std::is_integral_v<T> && std::is_unsigned_v<T>);
	#else
        false;
	#endif
	// clang-format on

	/**
	 * Wrapper for constexpr __builtin_abs functions.
	 * This should be used internally and always be wrapped in an if constexpr statement.
	 * It exists only to allow for usage of __builtin_abs functions without triggering a compiler error
	 * when the compiler does not support them.
	 */
	template <typename T>
	constexpr auto abs(T x)
		-> std::enable_if_t<has_constexpr_abs<T>, T>
	{
		if constexpr (std::is_same_v<T, float>)
		{
			return __builtin_fabsf(x);
		}
		else if constexpr (std::is_same_v<T, double>)
		{
			return __builtin_fabs(x);
		}
		else if constexpr (std::is_same_v<T, long double>)
		{
			return __builtin_fabsl(x);
		}
		else if constexpr (std::is_same_v<T, long long>)
		{
			return __builtin_llabs(x);
		}
		else if constexpr (std::is_integral_v<T> && std::is_signed_v<T>)
		{
			return __builtin_abs(x);
		}
		else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>)
		{
			return x; // Absolute value of unsigned is the value itself
		}
		// This should never be reached
		return T{};
	}
} // namespace ccm::builtin

// Cleanup the global namespace
#undef CCMATH_HAS_CONSTEXPR_BUILTIN_ABS
