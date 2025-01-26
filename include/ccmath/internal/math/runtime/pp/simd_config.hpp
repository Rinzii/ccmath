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

#include <utility> // For std::integer_sequence

// x86 macros

#ifdef __MMX__
	#define CCMATH_SIMD_HAVE_MMX 1
#else
	#define CCMATH_SIMD_HAVE_MMX 0
#endif

#if defined __SSE__ || defined __x86_64__
	#define CCMATH_SIMD_HAVE_SSE 1
#else
	#define CCMATH_SIMD_HAVE_SSE 0
#endif

#if defined __SSE2__ || defined __x86_64__
	#define CCMATH_SIMD_HAVE_SSE2 1
#else
	#define CCMATH_SIMD_HAVE_SSE2 0
#endif

#ifdef __SSE3__
	#define CCMATH_SIMD_HAVE_SSE3 1
#else
	#define CCMATH_SIMD_HAVE_SSE3 0
#endif

#ifdef __SSSE3__
	#define CCMATH_SIMD_HAVE_SSSE3 1
#else
	#define CCMATH_SIMD_HAVE_SSSE3 0
#endif

#ifdef __SSE4_1__
	#define CCMATH_SIMD_HAVE_SSE4_1 1
#else
	#define CCMATH_SIMD_HAVE_SSE4_1 0
#endif

#ifdef __SSE4_2__
	#define CCMATH_SIMD_HAVE_SSE4_2 1
#else
	#define CCMATH_SIMD_HAVE_SSE4_2 0
#endif

#ifdef __XOP__
	#define CCMATH_SIMD_HAVE_XOP 1
#else
	#define CCMATH_SIMD_HAVE_XOP 0
#endif

#ifdef __AVX__
	#define CCMATH_SIMD_HAVE_AVX 1
#else
	#define CCMATH_SIMD_HAVE_AVX 0
#endif

#ifdef __AVX2__
	#define CCMATH_SIMD_HAVE_AVX2 1
#else
	#define CCMATH_SIMD_HAVE_AVX2 0
#endif

#ifdef __BMI__
	#define CCMATH_SIMD_HAVE_BMI 1
#else
	#define CCMATH_SIMD_HAVE_BMI 0
#endif

#ifdef __BMI2__
	#define CCMATH_SIMD_HAVE_BMI2 1
#else
	#define CCMATH_SIMD_HAVE_BMI2 0
#endif

#ifdef __LZCNT__
	#define CCMATH_SIMD_HAVE_LZCNT 1
#else
	#define CCMATH_SIMD_HAVE_LZCNT 0
#endif

#ifdef __SSE4A__
	#define CCMATH_SIMD_HAVE_SSE4A 1
#else
	#define CCMATH_SIMD_HAVE_SSE4A 0
#endif

#ifdef __FMA__
	#define CCMATH_SIMD_HAVE_FMA 1
#else
	#define CCMATH_SIMD_HAVE_FMA 0
#endif

#ifdef __FMA4__
	#define CCMATH_SIMD_HAVE_FMA4 1
#else
	#define CCMATH_SIMD_HAVE_FMA4 0
#endif

#ifdef __F16C__
	#define CCMATH_SIMD_HAVE_F16C 1
#else
	#define CCMATH_SIMD_HAVE_F16C 0
#endif

#ifdef __POPCNT__
	#define CCMATH_SIMD_HAVE_POPCNT 1
#else
	#define CCMATH_SIMD_HAVE_POPCNT 0
#endif

#ifdef __AVX512F__
	#define CCMATH_SIMD_HAVE_AVX512F 1
#else
	#define CCMATH_SIMD_HAVE_AVX512F 0
#endif

#ifdef __AVX512DQ__
	#define CCMATH_SIMD_HAVE_AVX512DQ 1
#else
	#define CCMATH_SIMD_HAVE_AVX512DQ 0
#endif

#ifdef __AVX512VL__
	#define CCMATH_SIMD_HAVE_AVX512VL 1
#else
	#define CCMATH_SIMD_HAVE_AVX512VL 0
#endif

#ifdef __AVX512BW__
	#define CCMATH_SIMD_HAVE_AVX512BW 1
#else
	#define CCMATH_SIMD_HAVE_AVX512BW 0
#endif

#ifdef __AVX512BITALG__
	#define CCMATH_SIMD_HAVE_AVX512BITALG 1
#else
	#define CCMATH_SIMD_HAVE_AVX512BITALG 0
#endif

#ifdef __AVX512VBMI2__
	#define CCMATH_SIMD_HAVE_AVX512VBMI2 1
#else
	#define CCMATH_SIMD_HAVE_AVX512VBMI2 0
#endif

#ifdef __AVX512VBMI__
	#define CCMATH_SIMD_HAVE_AVX512VBMI 1
#else
	#define CCMATH_SIMD_HAVE_AVX512VBMI 0
#endif

#ifdef __AVX512IFMA__
	#define CCMATH_SIMD_HAVE_AVX512IFMA 1
#else
	#define CCMATH_SIMD_HAVE_AVX512IFMA 0
#endif

#ifdef __AVX512CD__
	#define CCMATH_SIMD_HAVE_AVX512CD 1
#else
	#define CCMATH_SIMD_HAVE_AVX512CD 0
#endif

#ifdef __AVX512VNNI__
	#define CCMATH_SIMD_HAVE_AVX512VNNI 1
#else
	#define CCMATH_SIMD_HAVE_AVX512VNNI 0
#endif

#ifdef __AVX512VPOPCNTDQ__
	#define CCMATH_SIMD_HAVE_AVX512VPOPCNTDQ 1
#else
	#define CCMATH_SIMD_HAVE_AVX512VPOPCNTDQ 0
#endif

#ifdef __AVX512VP2INTERSECT__
	#define CCMATH_SIMD_HAVE_AVX512VP2INTERSECT 1
#else
	#define CCMATH_SIMD_HAVE_AVX512VP2INTERSECT 0
#endif

#ifdef __AVX512FP16__
	#define CCMATH_SIMD_HAVE_AVX512FP16 1
#else
	#define CCMATH_SIMD_HAVE_AVX512FP16 0
#endif

#if CCMATH_SIMD_HAVE_SSE
	#define CCMATH_SIMD_HAVE_SSE_ABI 1
#else
	#define CCMATH_SIMD_HAVE_SSE_ABI 0
#endif

#if CCMATH_SIMD_HAVE_SSE2
	#define CCMATH_SIMD_HAVE_FULL_SSE_ABI 1
#else
	#define CCMATH_SIMD_HAVE_FULL_SSE_ABI 0
#endif

#if CCMATH_SIMD_HAVE_AVX
	#define CCMATH_SIMD_HAVE_AVX_ABI 1
#else
	#define CCMATH_SIMD_HAVE_AVX_ABI 0
#endif

#if CCMATH_SIMD_HAVE_AVX2
	#define CCMATH_SIMD_HAVE_FULL_AVX_ABI 1
#else
	#define CCMATH_SIMD_HAVE_FULL_AVX_ABI 0
#endif

#if CCMATH_SIMD_HAVE_AVX512F
	#define CCMATH_SIMD_HAVE_AVX512_ABI 1
#else
	#define CCMATH_SIMD_HAVE_AVX512_ABI 0
#endif

#if CCMATH_SIMD_HAVE_AVX512BW
	#define CCMATH_SIMD_HAVE_FULL_AVX512_ABI 1
#else
	#define CCMATH_SIMD_HAVE_FULL_AVX512_ABI 0
#endif

// Intel Short Vector Math Library (SVML)
// As far as I am aware, there is no reliable way to detect SVML support at compile-time.
#if defined(CCM_CONFIG_RT_SIMD_HAS_SVML)
	#ifndef CCMATH_HAS_SIMD
		#define CCMATH_HAS_SIMD 1
	#endif
	#define CCMATH_HAS_SIMD_SVML 1
#endif

#if defined __x86_64__ && !CCMATH_SIMD_HAVE_SSE2
	#error "Use of SSE2 is required on AMD64"
#endif

// ARM macros
#if defined __ARM_NEON
	#define CCMATH_SIMD_HAVE_NEON 1
#else
	#define CCMATH_SIMD_HAVE_NEON 0
#endif
#if defined __ARM_NEON && (__ARM_ARCH >= 8 || defined __aarch64__)
	#define CCMATH_SIMD_HAVE_NEON_A32 1
#else
	#define CCMATH_SIMD_HAVE_NEON_A32 0
#endif
#if defined __ARM_NEON && defined __aarch64__
	#define CCMATH_SIMD_HAVE_NEON_A64 1
#else
	#define CCMATH_SIMD_HAVE_NEON_A64 0
#endif
#if (__ARM_FEATURE_SVE_BITS > 0 && __ARM_FEATURE_SVE_VECTOR_OPERATORS == 1)
	#define CCMATH_SIMD_HAVE_SVE 1
#else
	#define CCMATH_SIMD_HAVE_SVE 0
#endif

// CCM_SIMD_INTRINSIC
#if defined(CCM_CONFIG_NO_SIMD_INLINE)
	#define CCM_SIMD_INTRINSIC
#elif defined(CCM_CONFIG_NO_FORCED_SIMD_INLINE)
	#define CCM_SIMD_INTRINSIC inline
#elif defined(__GNUC__) || (defined(__clang__) && !defined(_MSC_VER))
	#define CCM_SIMD_INTRINSIC __attribute__((always_inline, artificial)) inline
#elif defined(_MSC_VER)
	#define CCM_SIMD_INTRINSIC __forceinline
#else
	#define CCM_SIMD_INTRINSIC inline
#endif

// CCM_SIMD_ENFORCED_ALWAYS_INLINE
#if defined(__GNUC__) || (defined(__clang__) && !defined(_MSC_VER))
	#define CCM_SIMD_ENFORCED_ALWAYS_INLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
	#define CCM_SIMD_ENFORCED_ALWAYS_INLINE __forceinline
#else
	#define CCM_SIMD_ENFORCED_ALWAYS_INLINE inline
#endif

#ifndef CCMATH_SIMD_LIST_BINARY
	#define CCMATH_SIMD_LIST_BINARY(MACRO)		MACRO(|) MACRO(&) MACRO(^)
	#define CCMATH_SIMD_LIST_SHIFTS(MACRO)		MACRO(<<) MACRO(>>)
	#define CCMATH_SIMD_LIST_ARITHMETICS(MACRO) MACRO(+) MACRO(-) MACRO(*) MACRO(/) MACRO(%)

	#define CCMATH_SIMD_ALL_BINARY(MACRO)	   CCMATH_SIMD_LIST_BINARY(MACRO) static_assert(true)
	#define CCMATH_SIMD_ALL_SHIFTS(MACRO)	   CCMATH_SIMD_LIST_SHIFTS(MACRO) static_assert(true)
	#define CCMATH_SIMD_ALL_ARITHMETICS(MACRO) CCMATH_SIMD_LIST_ARITHMETICS(MACRO) static_assert(true)
#endif

#if defined __GXX_CONDITIONAL_IS_OVERLOADABLE__ && SIMD_CONDITIONAL_OPERATOR
	#define simd_select_impl operator?:
#endif

namespace ccm::pp::config::detail
{
	template <int... PACK, typename F>
	CCM_SIMD_ENFORCED_ALWAYS_INLINE constexpr void simd_int_pack(std::integer_sequence<int, PACK...>, F && code)
	{
		code(std::integer_sequence<int, PACK...>{});
	}
} // namespace ccm::pp::config::detail

#define CCMATH_SIMD_INT_PACK(N, CODE) ccm::pp::config::detail::simd_int_pack(std::make_integer_sequence<int, N>{}, CODE)
