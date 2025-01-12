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

/// CCMATH_HAS_CONSTEXPR_BUILTIN_ERFC
/// This is a macro that is defined if the compiler has constexpr __builtin_erfc that allows static_assert
///
/// Compilers with Support:
/// - GCC 5.1+

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_ERFC
	#if defined(__GNUC__) && (__GNUC__ > 5 || (__GNUC__ == 5 && __GNUC_MINOR__ >= 1)) && !defined(__clang__) && !defined(__NVCOMPILER_MAJOR__)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_ERFC
	#endif
#endif

namespace ccm::builtin
{
	// clang-format off
	/**
	 * @internal
	 */
	template <typename T>
	inline constexpr bool has_constexpr_erfc =
#ifdef CCMATH_HAS_CONSTEXPR_BUILTIN_ERFC
		is_valid_builtin_type<T>;
	#else
			false;
	#endif
	// clang-format on

	/**
	 * @internal
	 * Wrapper for constexpr __builtin_erfc functions.
	 * This should be used internally and always be wrapped in an if constexpr statement.
	 * It exists only to allow for usage of __builtin_erfc without triggering a compiler error
	 * when the compiler does not support it.
	 */
	template <typename T>
	constexpr auto erfc(T x) -> std::enable_if_t<has_constexpr_erfc<T>, T>
	{
		if constexpr (std::is_same_v<T, float>) { return __builtin_erfcf(x); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_erfc(x); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_erfcl(x); }
		else
		{
			// This should never be reached
			static_assert(ccm::support::always_false<T>, "Unsupported type for __builtin_erfc");
			return T{};
		}
	}
} // namespace ccm::builtin

// Cleanup the global namespace
#undef CCMATH_HAS_CONSTEXPR_BUILTIN_ERFC
