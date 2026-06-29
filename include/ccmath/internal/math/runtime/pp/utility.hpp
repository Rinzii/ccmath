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

#include "ccmath/internal/math/runtime/pp/declaration.hpp"
#include "ccmath/internal/predef/attributes/always_inline.hpp"

#include <cstddef>
#include <utility>

namespace ccm::pp
{
	// Per-(T, Abi) storage and operation set. Specialized by the scalar and
	// vec_ext backends. This is the libc++ __simd_storage + __simd_operations +
	// __mask_operations rolled into one trait, keyed on the public ABI tag.
	template <typename T, typename Abi> struct SimdTraits;

	namespace detail
	{
		// Compile-time unrolled apply over the lane indices [0, N). The callback
		// receives a plain index so it can be used to subscript a builtin vector.
		template <typename F, std::size_t... Is>
		CCM_ALWAYS_INLINE constexpr void unroll_impl(F && f, std::index_sequence<Is...> /*unused*/) // NOLINT(cppcoreguidelines-missing-std-forward)
		{
			(f(static_cast<SimdSizeType>(Is)), ...);
		}

		template <SimdSizeType N, typename F> CCM_ALWAYS_INLINE constexpr void unroll(F && f)
		{
			unroll_impl(std::forward<F>(f), std::make_index_sequence<static_cast<std::size_t>(N)>{});
		}

		// As unroll, but passes a std::integral_constant so the callback can use
		// the index in a constant-expression context (used by the generator ctor).
		template <typename F, std::size_t... Is>
		CCM_ALWAYS_INLINE constexpr void unroll_ic_impl(F && f, std::index_sequence<Is...> /*unused*/) // NOLINT(cppcoreguidelines-missing-std-forward)
		{
			(f(std::integral_constant<SimdSizeType, static_cast<SimdSizeType>(Is)>{}), ...);
		}

		template <SimdSizeType N, typename F> CCM_ALWAYS_INLINE constexpr void unroll_ic(F && f)
		{
			unroll_ic_impl(std::forward<F>(f), std::make_index_sequence<static_cast<std::size_t>(N)>{});
		}

		// Per-lane scalar math primitives for the scalar backend and the vec_ext
		// portable fallback. GNU/Clang use foldable, errno-free compiler builtins
		// (no <cmath>). Other toolchains (e.g. MSVC) or CCM_PP_FORCE_PORTABLE builds
		// call the C runtime entry points declared below. CCM_PP_F and CCM_PP_D
		// select the float and double backend for a given function base name.
#if (defined(__GNUC__) || defined(__clang__)) && !defined(CCM_PP_FORCE_PORTABLE)
	#define CCM_PP_F(BASE) __builtin_##BASE##f
	#define CCM_PP_D(BASE) __builtin_##BASE
#else
		extern "C" {
		float sqrtf(float);
		double sqrt(double);
		float floorf(float);
		double floor(double);
		float ceilf(float);
		double ceil(double);
		float truncf(float);
		double trunc(double);
		float roundf(float);
		double round(double);
		double fabs(double);
		float expf(float);
		double exp(double);
		float logf(float);
		double log(double);
		float fmaf(float, float, float);
		double fma(double, double, double);
		float powf(float, float);
		double pow(double, double);
		}
	#define CCM_PP_F(BASE) BASE##f
	#define CCM_PP_D(BASE) BASE
#endif

#define CCM_PP_S_UNARY(NAME, BASE)                                                                                                                             \
	template <typename T> CCM_ALWAYS_INLINE T NAME(T x)                                                                                                        \
	{                                                                                                                                                          \
		if constexpr (std::is_same_v<T, float>)                                                                                                                \
		{                                                                                                                                                      \
			return CCM_PP_F(BASE)(x);                                                                                                                          \
		} else                                                                                                                                                 \
		{                                                                                                                                                      \
			return CCM_PP_D(BASE)(x);                                                                                                                          \
		}                                                                                                                                                      \
	}
		CCM_PP_S_UNARY(s_sqrt, sqrt)
		CCM_PP_S_UNARY(s_floor, floor)
		CCM_PP_S_UNARY(s_ceil, ceil)
		CCM_PP_S_UNARY(s_trunc, trunc)
		CCM_PP_S_UNARY(s_round, round)
		CCM_PP_S_UNARY(s_exp, exp)
		CCM_PP_S_UNARY(s_log, log)
#undef CCM_PP_S_UNARY

		template <typename T> CCM_ALWAYS_INLINE T s_fma(T a, T b, T c)
		{
			if constexpr (std::is_same_v<T, float>)
			{
				return CCM_PP_F(fma)(a, b, c);
			} else
			{
				return CCM_PP_D(fma)(a, b, c);
			}
		}
		template <typename T> CCM_ALWAYS_INLINE T s_pow(T a, T b)
		{
			if constexpr (std::is_same_v<T, float>)
			{
				return CCM_PP_F(pow)(a, b);
			} else
			{
				return CCM_PP_D(pow)(a, b);
			}
		}

#undef CCM_PP_F
#undef CCM_PP_D

		// MSVC does not always provide a linkable fabsf, so on the portable path the
		// float case routes through the double fabs, which is exact because fabs only
		// clears the sign bit.
		template <typename T> CCM_ALWAYS_INLINE T s_fabs(T x)
		{
#if (defined(__GNUC__) || defined(__clang__)) && !defined(CCM_PP_FORCE_PORTABLE)
			if constexpr (std::is_same_v<T, float>)
			{
				return __builtin_fabsf(x);
			} else
			{
				return __builtin_fabs(x);
			}
#else
			if constexpr (std::is_same_v<T, float>)
			{
				return static_cast<float>(fabs(static_cast<double>(x)));
			} else
			{
				return fabs(x);
			}
#endif
		}
	} // namespace detail
} // namespace ccm::pp
