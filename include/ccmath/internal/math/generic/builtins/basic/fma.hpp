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
#include "ccmath/internal/predef/has_builtin.hpp"
#include "ccmath/internal/support/always_false.hpp"

// Checks if the cpu supports FMA instructions.
// This macro is allowed to be defined in the global namespace.
#if defined(__ARM_FEATURE_FMA) || (defined(__AVX2__) && defined(__FMA__)) || defined(__NVPTX__) || defined(__AMDGPU__) ||                                      \
	(defined(__riscv) && (__riscv_xlen == 64))
	#define CCMATH_TARGET_CPU_HAS_FMA
#endif

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
	/**
	 * @internal
	 */
	template <typename T>
	inline constexpr bool has_constexpr_fma =
		#ifdef CCMATH_HAS_CONSTEXPR_BUILTIN_FMA
			is_valid_builtin_type<T>;
		#else
			false;
		#endif
	/**
	 * @internal
	 */
	template <typename T>
	inline constexpr bool has_fma =
		#if CCM_HAS_BUILTIN(__builtin_fma) || CCM_HAS_BUILTIN(__builtin_fmaf) || CCM_HAS_BUILTIN(__builtin_fmal)
			is_valid_builtin_type<T>;
		#else
			false;
		#endif
	// clang-format on

	/**
	 * @internal
	 * Wrapper for constexpr __builtin_fma.
	 * This should be used internally and always be wrapped in an if constexpr statement.
	 * It exists only to allow for usage of __builtin_fma without triggering a compiler error
	 * when the compiler does not support it.
	 * This is thanks to taking advantage of ADL.
	 */
	template <typename T>
	constexpr auto fma(T x, T y, T z) -> std::enable_if_t<has_constexpr_fma<T>, T>
	{
		if constexpr (std::is_same_v<T, float>) { return __builtin_fmaf(x, y, z); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_fma(x, y, z); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_fmal(x, y, z); }
		else
		{
			// This should never be reached
			static_assert(ccm::support::always_false<T>, "Unsupported type for __builtin_fma");
			return T{};
		}
	}

	/**
	 * @internal
	 * Wrapper for runtime __builtin_fma.
	 * This should be used internally and always be wrapped in an if constexpr statement.
	 * It exists only to allow for usage of __builtin_fma without triggering a compiler error
	 * when the compiler does not support it.
	 * This is thanks to taking advantage of ADL.
	 */
	template <typename T>
	auto fma(T x, T y, T z) -> std::enable_if_t<has_fma<T>, T>
	{
		if constexpr (std::is_same_v<T, float>) { return __builtin_fmaf(x, y, z); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_fma(x, y, z); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_fmal(x, y, z); }
		else
		{
			// This should never be reached
			static_assert(ccm::support::always_false<T>, "Unsupported type for __builtin_fma");
			return T{};
		}
	}
} // namespace ccm::builtin

// Cleanup the global namespace
#undef CCMATH_HAS_CONSTEXPR_BUILTIN_FMA
