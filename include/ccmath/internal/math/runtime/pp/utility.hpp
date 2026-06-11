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
	template <typename T, typename Abi>
	struct SimdTraits;

	namespace detail
	{
		// Compile-time unrolled apply over the lane indices [0, N). The callback
		// receives a plain index so it can be used to subscript a builtin vector.
		template <typename F, std::size_t... Is>
		CCM_ALWAYS_INLINE constexpr void unroll_impl(F && f, std::index_sequence<Is...> /*unused*/)
		{ (f(static_cast<SimdSizeType>(Is)), ...); }

		template <SimdSizeType N, typename F>
		CCM_ALWAYS_INLINE constexpr void unroll(F && f)
		{ unroll_impl(std::forward<F>(f), std::make_index_sequence<static_cast<std::size_t>(N)>{}); }

		// As unroll, but passes a std::integral_constant so the callback can use
		// the index in a constant-expression context (used by the generator ctor).
		template <typename F, std::size_t... Is>
		CCM_ALWAYS_INLINE constexpr void unroll_ic_impl(F && f, std::index_sequence<Is...> /*unused*/)
		{ (f(std::integral_constant<SimdSizeType, static_cast<SimdSizeType>(Is)>{}), ...); }

		template <SimdSizeType N, typename F>
		CCM_ALWAYS_INLINE constexpr void unroll_ic(F && f)
		{ unroll_ic_impl(std::forward<F>(f), std::make_index_sequence<static_cast<std::size_t>(N)>{}); }

		// Per-lane scalar math primitives, used by the scalar backend and as the
		// portable fallback for the vec_ext backend. Compiler builtins are used on
		// GNU/Clang (no <cmath>, no errno, foldable); other toolchains (e.g. MSVC),
		// or builds that define CCM_PP_FORCE_PORTABLE, fall back to the C runtime
		// entry points declared here.
#if (defined(__GNUC__) || defined(__clang__)) && !defined(CCM_PP_FORCE_PORTABLE)
	#define CCM_PP_S_UNARY(NAME, BF, BD)                                                                                                                       \
		template <typename T>                                                                                                                                  \
		CCM_ALWAYS_INLINE T NAME(T x)                                                                                                                          \
		{                                                                                                                                                      \
			if constexpr (std::is_same<T, float>::value) { return BF(x); }                                                                                     \
			else                                                                                                                                               \
			{                                                                                                                                                  \
				return BD(x);                                                                                                                                  \
			}                                                                                                                                                  \
		}
		CCM_PP_S_UNARY(s_sqrt, __builtin_sqrtf, __builtin_sqrt)
		CCM_PP_S_UNARY(s_floor, __builtin_floorf, __builtin_floor)
		CCM_PP_S_UNARY(s_ceil, __builtin_ceilf, __builtin_ceil)
		CCM_PP_S_UNARY(s_trunc, __builtin_truncf, __builtin_trunc)
		CCM_PP_S_UNARY(s_round, __builtin_roundf, __builtin_round)
		CCM_PP_S_UNARY(s_fabs, __builtin_fabsf, __builtin_fabs)
		CCM_PP_S_UNARY(s_exp, __builtin_expf, __builtin_exp)
		CCM_PP_S_UNARY(s_log, __builtin_logf, __builtin_log)
	#undef CCM_PP_S_UNARY

		template <typename T>
		CCM_ALWAYS_INLINE T s_fma(T a, T b, T c)
		{
			if constexpr (std::is_same<T, float>::value) { return __builtin_fmaf(a, b, c); }
			else
			{
				return __builtin_fma(a, b, c);
			}
		}
		template <typename T>
		CCM_ALWAYS_INLINE T s_pow(T a, T b)
		{
			if constexpr (std::is_same<T, float>::value) { return __builtin_powf(a, b); }
			else
			{
				return __builtin_pow(a, b);
			}
		}
#elif defined(_MSC_VER)
	#include <math.h>
	#define CCM_PP_S_UNARY(NAME, CF, CD)                                                                                                                       \
		template <typename T>                                                                                                                                  \
		CCM_ALWAYS_INLINE T NAME(T x)                                                                                                                          \
		{                                                                                                                                                      \
			if constexpr (std::is_same<T, float>::value) { return ::CF(x); }                                                                                   \
			else                                                                                                                                               \
			{                                                                                                                                                  \
				return ::CD(x);                                                                                                                                \
			}                                                                                                                                                  \
		}
		CCM_PP_S_UNARY(s_sqrt, sqrtf, sqrt)
		CCM_PP_S_UNARY(s_floor, floorf, floor)
		CCM_PP_S_UNARY(s_ceil, ceilf, ceil)
		CCM_PP_S_UNARY(s_trunc, truncf, trunc)
		CCM_PP_S_UNARY(s_round, roundf, round)
		CCM_PP_S_UNARY(s_fabs, fabsf, fabs)
		CCM_PP_S_UNARY(s_exp, expf, exp)
		CCM_PP_S_UNARY(s_log, logf, log)
	#undef CCM_PP_S_UNARY

		template <typename T>
		CCM_ALWAYS_INLINE T s_fma(T a, T b, T c)
		{
			if constexpr (std::is_same<T, float>::value) { return ::fmaf(a, b, c); }
			else
			{
				return ::fma(a, b, c);
			}
		}
		template <typename T>
		CCM_ALWAYS_INLINE T s_pow(T a, T b)
		{
			if constexpr (std::is_same<T, float>::value) { return ::powf(a, b); }
			else
			{
				return ::pow(a, b);
			}
		}
#else
		extern "C"
		{
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
			float fabsf(float);
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
	#define CCM_PP_S_UNARY(NAME, CF, CD)                                                                                                                       \
		template <typename T>                                                                                                                                  \
		CCM_ALWAYS_INLINE T NAME(T x)                                                                                                                          \
		{                                                                                                                                                      \
			if constexpr (std::is_same<T, float>::value) { return CF(x); }                                                                                     \
			else                                                                                                                                               \
			{                                                                                                                                                  \
				return CD(x);                                                                                                                                  \
			}                                                                                                                                                  \
		}
		CCM_PP_S_UNARY(s_sqrt, sqrtf, sqrt)
		CCM_PP_S_UNARY(s_floor, floorf, floor)
		CCM_PP_S_UNARY(s_ceil, ceilf, ceil)
		CCM_PP_S_UNARY(s_trunc, truncf, trunc)
		CCM_PP_S_UNARY(s_round, roundf, round)
		CCM_PP_S_UNARY(s_fabs, fabsf, fabs)
		CCM_PP_S_UNARY(s_exp, expf, exp)
		CCM_PP_S_UNARY(s_log, logf, log)
	#undef CCM_PP_S_UNARY

		template <typename T>
		CCM_ALWAYS_INLINE T s_fma(T a, T b, T c)
		{
			if constexpr (std::is_same<T, float>::value) { return fmaf(a, b, c); }
			else
			{
				return fma(a, b, c);
			}
		}
		template <typename T>
		CCM_ALWAYS_INLINE T s_pow(T a, T b)
		{
			if constexpr (std::is_same<T, float>::value) { return powf(a, b); }
			else
			{
				return pow(a, b);
			}
		}
#endif
	} // namespace detail
} // namespace ccm::pp
