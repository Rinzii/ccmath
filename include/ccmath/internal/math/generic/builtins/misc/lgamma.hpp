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
#include "ccmath/internal/support/always_false.hpp"

#include <type_traits>

/// CCMATH_HAS_BUILTIN_LGAMMA
/// This is a macro that is defined if the compiler has constexpr __builtin functions for lgamma that allow static_assert
///
/// Compilers with Support:
/// - GCC
/// - Clang

// TODO(IanP): Determine the lowest runtime compiler versions at some point.

#ifndef CCMATH_HAS_BUILTIN_LGAMMA
	#if defined(__GNUC__) || defined(__clang__)
		#define CCMATH_HAS_BUILTIN_LGAMMA
	#endif
#endif

namespace ccm::builtin
{
	// TODO: determine actual compiler/version support for runtime __builtin_lgamma.
	template <typename T>
	inline constexpr bool has_runtime_lgamma =
#ifdef CCMATH_HAS_BUILTIN_LGAMMA
		is_valid_transcendental_builtin_type<T>;
#else
		false;
#endif

	template <typename T>
	auto lgamma_rt(T x) -> std::enable_if_t<has_runtime_lgamma<T>, T>
	{
		if constexpr (std::is_same_v<T, float>) { return __builtin_lgammaf(x); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_lgamma(x); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_lgammal(x); }
		else
		{
			static_assert(ccm::support::always_false<T>, "Unsupported type for lgamma");
			return T{};
		}
	}
} // namespace ccm::builtin

// Cleanup the global namespace
#undef CCMATH_HAS_BUILTIN_LGAMMA
