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

#include "ccmath/internal/config/compiler.hpp"
#include "ccmath/internal/config/type_support.hpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "ccmath/internal/math/generic/builtins/builtin_helpers.hpp"
#include "ccmath/internal/predef/has_builtin.hpp"
#include "ccmath/internal/support/always_false.hpp"

#include <type_traits>

// Checks if the cpu supports FMA instructions.
// This macro is allowed to be defined in the global namespace.
#if defined(__ARM_FEATURE_FMA) || (defined(__AVX2__) && defined(__FMA__)) || defined(__NVPTX__) || defined(__AMDGPU__) ||                                      \
	(defined(__riscv) && (__riscv_xlen == 64))
	#define CCMATH_TARGET_CPU_HAS_FMA
#endif

// Deterministic mode routes every multiply_add through the IEEE-correct __builtin_fma (hardware
// where present, a software fma() call otherwise) so the result is bit-identical on FMA and
// non-FMA targets, and forces the if constexpr(target_cpu_has_fma) kernel branches to a single
// path. This does not assert that the CPU has an FMA instruction. It selects the correctly-rounded
// fused operation regardless of hardware.
#if defined(CCM_CONFIG_DETERMINISTIC) && !defined(CCMATH_TARGET_CPU_HAS_FMA)
	#define CCMATH_TARGET_CPU_HAS_FMA
#endif

/// CCMATH_HAS_CONSTEXPR_BUILTIN_FMA
/// This is a macro that is defined if the compiler has constexpr __builtin functions for fma that allow static_assert
///
/// Compilers with Support:
/// - GCC 5.1+

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_FMA
	#if defined(__GNUC__) && (__GNUC__ > 5 || (__GNUC__ == 5 && __GNUC_MINOR__ >= 1)) && !defined(__clang__) && !defined(__NVCOMPILER_MAJOR__)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_FMA
	#endif
#endif

/// CCMATH_HAS_BUILTIN_FMA
/// This is a macro that is defined if the compiler has constexpr __builtin functions for fma that allow static_assert
///
/// Compilers with Support:
/// - GCC
/// - Clang

// TODO(IanP): Determine the lowest runtime compiler versions at some point.

#ifndef CCMATH_HAS_BUILTIN_FMA
	#if defined(__GNUC__) || defined(__clang__)
		#define CCMATH_HAS_BUILTIN_FMA
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

	// TODO: determine actual compiler/version support for runtime __builtin_fma.
	template <typename T>
	inline constexpr bool has_runtime_fma =
#ifdef CCMATH_HAS_BUILTIN_FMA
		is_valid_builtin_type<T>;
#else
		false;
#endif

	inline constexpr bool target_cpu_has_fma =
#ifdef CCMATH_TARGET_CPU_HAS_FMA
		true
#else
		false
#endif
		;

	inline constexpr bool runtime_builtin_fma_enabled =
#ifdef CCM_CONFIG_DISABLE_RUNTIME_BUILTIN_FMA
		false
#else
		true
#endif
		;

	inline constexpr bool runtime_builtin_fma_validated_target =
#if (defined(__aarch64__) || defined(__arm64__)) && (defined(CCMATH_COMPILER_APPLE_CLANG) || defined(CCMATH_COMPILER_CLANG) || defined(CCMATH_COMPILER_GCC))
		true
#else
		false
#endif
		;

	template <typename T>
	inline constexpr bool runtime_builtin_fma_long_double_supported =
#ifdef CCM_TYPES_LONG_DOUBLE_IS_FLOAT64
		true
#else
		!std::is_same_v<T, long double>
#endif
		;

	template <typename T>
	inline constexpr bool runtime_builtin_fma_trusted =
		runtime_builtin_fma_enabled && has_fma<T> && runtime_builtin_fma_validated_target && runtime_builtin_fma_long_double_supported<T>;

	/**
	 * @internal
	 * Wrapper for constexpr __builtin_fma.
	 * This should be used internally and always be wrapped in an if constexpr statement.
	 * It exists only to allow for usage of __builtin_fma without triggering a compiler error
	 * when the compiler does not support it.
	 * This is thanks to taking advantage of ADL.
	 */
	template <typename T> constexpr auto fma_ct(T x, T y, T z) -> std::enable_if_t<has_constexpr_fma<T>, T>
	{
		if constexpr (std::is_same_v<T, float>)
		{
			return __builtin_fmaf(x, y, z);
		} else if constexpr (std::is_same_v<T, double>)
		{
			return __builtin_fma(x, y, z);
		} else if constexpr (std::is_same_v<T, long double>)
		{
			return __builtin_fmal(x, y, z);
		} else
		{
			// This should never be reached
			static_assert(ccm::support::always_false<T>, "Unsupported type for fma");
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
	template <typename T> auto fma_rt(T x, T y, T z) -> std::enable_if_t<has_runtime_fma<T>, T>
	{
		if constexpr (std::is_same_v<T, float>)
		{
			return __builtin_fmaf(x, y, z);
		} else if constexpr (std::is_same_v<T, double>)
		{
			return __builtin_fma(x, y, z);
		} else if constexpr (std::is_same_v<T, long double>)
		{
			return __builtin_fmal(x, y, z);
		} else
		{
			// This should never be reached
			static_assert(ccm::support::always_false<T>, "Unsupported type for fma");
			return T{};
		}
	}

} // namespace ccm::builtin

// Cleanup the global namespace
#undef CCMATH_HAS_CONSTEXPR_BUILTIN_FMA
#undef CCMATH_HAS_BUILTIN_FMA
