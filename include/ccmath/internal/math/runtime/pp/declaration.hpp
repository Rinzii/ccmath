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

#include "ccmath/internal/math/runtime/pp/simd_config.hpp"

#include <cstddef>
#include <cstdint>
#include <type_traits>

// On MSVC (cl.exe) the Clang/GCC vector extensions are unavailable, so instead
// of the scalar array fallback we use an <immintrin.h> intrinsic backend for the
// 128-bit native widths. CCM_PP_FORCE_MSVC_INTRIN forces that path on any x86
// compiler (used to validate it on Clang). Only 128-bit is implemented, so
// native deduction is capped accordingly.
#if defined(CCM_PP_FORCE_MSVC_INTRIN) ||                                                                                                                       \
	(defined(_MSC_VER) && !defined(__clang__) && !defined(CCM_PP_FORCE_PORTABLE) && !defined(CCM_PP_FORCE_ARRAY_BACKEND) && CCMATH_SIMD_HAVE_SSE2)
	#define CCM_PP_USE_MSVC_INTRIN 1
#else
	#define CCM_PP_USE_MSVC_INTRIN 0
#endif

// Forward declarations, ABI tags and ABI deduction for the C++17 port of the
// C++26 std::simd surface. The internal layout follows libc++'s
// <experimental/simd> (declaration / scalar / vec_ext backends) while exposing
// the C++26 public names (basic_simd, basic_simd_mask, ...).

namespace ccm::pp
{
	namespace detail
	{
		// std::simd uses a signed size type for lane counts and indices.
		using SimdSizeType = int;

		// Signed integer type used to represent a mask lane of the given byte width.
		template <std::size_t Bytes>
		struct mask_int_from;
		template <>
		struct mask_int_from<1>
		{
			using type = std::int8_t;
		};
		template <>
		struct mask_int_from<2>
		{
			using type = std::int16_t;
		};
		template <>
		struct mask_int_from<4>
		{
			using type = std::int32_t;
		};
		template <>
		struct mask_int_from<8>
		{
			using type = std::int64_t;
		};

		template <std::size_t Bytes>
		using mask_integer_from = typename mask_int_from<Bytes>::type;

		// A type is vectorizable when std::simd may form a data-parallel object of
		// it. long double is intentionally excluded: it has no vector_size form.
		template <typename T>
		struct is_vectorizable : std::false_type
		{
		};
		// clang-format off
		template <> struct is_vectorizable<char>               : std::true_type {};
		template <> struct is_vectorizable<wchar_t>            : std::true_type {};
		template <> struct is_vectorizable<char16_t>          : std::true_type {};
		template <> struct is_vectorizable<char32_t>          : std::true_type {};
		template <> struct is_vectorizable<signed char>       : std::true_type {};
		template <> struct is_vectorizable<unsigned char>     : std::true_type {};
		template <> struct is_vectorizable<short>             : std::true_type {};
		template <> struct is_vectorizable<unsigned short>    : std::true_type {};
		template <> struct is_vectorizable<int>               : std::true_type {};
		template <> struct is_vectorizable<unsigned int>      : std::true_type {};
		template <> struct is_vectorizable<long>              : std::true_type {};
		template <> struct is_vectorizable<unsigned long>     : std::true_type {};
		template <> struct is_vectorizable<long long>         : std::true_type {};
		template <> struct is_vectorizable<unsigned long long>: std::true_type {};
		template <> struct is_vectorizable<float>             : std::true_type {};
		template <> struct is_vectorizable<double>            : std::true_type {};
		// clang-format on
#if defined(__cpp_char8_t) && __cpp_char8_t >= 201811L
		template <>
		struct is_vectorizable<char8_t> : std::true_type
		{
		};
#endif

		template <typename T>
		inline constexpr bool is_vectorizable_v = is_vectorizable<T>::value;

		constexpr bool is_pow2(SimdSizeType n)
		{ return n > 0 && (n & (n - 1)) == 0; }
	} // namespace detail

	// [simd.abi] ABI tags. ScalarAbi holds a single element. VecAbi<N> holds N
	// lanes backed by a compiler vector extension (or an array fallback).
	struct ScalarAbi
	{
		template <typename>
		struct IsValid : std::true_type
		{
		};
		static constexpr detail::SimdSizeType size = 1;
	};

	template <detail::SimdSizeType N>
	struct VecAbi
	{
		template <typename T>
		struct IsValid
			: std::bool_constant<detail::is_vectorizable_v<T> && N >= 2 && detail::is_pow2(N) && (N * static_cast<detail::SimdSizeType>(sizeof(T))) <= 64>
		{
		};
		static constexpr detail::SimdSizeType size = N;
	};

	namespace detail
	{
		// Native register width in bytes for the target the translation unit is
		// being compiled for. Drives NativeAbi lane selection. Native deduction is
		// capped at AVX (256-bit): AVX-512 is not selected by default (it always
		// also reports __AVX__, so those targets fall back to 256-bit here). Wider
		// widths remain available through an explicit VecAbi<N>.
		constexpr SimdSizeType native_simd_bytes()
		{
#if CCM_PP_USE_MSVC_INTRIN
	#if CCMATH_SIMD_HAVE_AVX
			return 32; // 256-bit AVX/AVX2 intrinsic backend
	#else
			return 16; // 128-bit SSE2 intrinsic backend
	#endif
#elif CCMATH_SIMD_HAVE_AVX
			return 32;
#elif CCMATH_SIMD_HAVE_SSE2 || CCMATH_SIMD_HAVE_NEON
			return 16;
#else
			return 0;
#endif
		}

		template <typename T, bool = is_vectorizable_v<T>>
		struct NativeAbiImpl
		{
			using type = ScalarAbi;
		};
		template <typename T>
		struct NativeAbiImpl<T, true>
		{
			static constexpr SimdSizeType bytes = native_simd_bytes();
			static constexpr SimdSizeType lanes = bytes / static_cast<SimdSizeType>(sizeof(T));
			using type							= std::conditional_t<(lanes >= 2 && VecAbi<lanes>::template IsValid<T>::value), VecAbi<lanes>, ScalarAbi>;
		};

		template <typename T>
		using NativeAbi = typename NativeAbiImpl<T>::type;

		// deduce_t<T, N> selects the ABI for an N-wide simd<T, N> alias.
		template <typename T, SimdSizeType N>
		struct DeduceAbi
		{
			using type = std::conditional_t<N == 1, ScalarAbi, VecAbi<N>>;
		};

		template <typename T, SimdSizeType N>
		using deduce_t = typename DeduceAbi<T, N>::type;
	} // namespace detail

	template <typename T, typename Abi = detail::NativeAbi<T>>
	class basic_simd;

	template <std::size_t Bytes, typename Abi = detail::NativeAbi<detail::mask_integer_from<Bytes>>>
	class basic_simd_mask;

	template <typename MaskType, typename SimdType>
	class const_where_expression;

	template <typename MaskType, typename SimdType>
	class where_expression;
} // namespace ccm::pp
