/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/arch/check_simd_support.hpp"
#include "ccmath/internal/runtime/simd/intrin_include.hpp"
#include "ccmath/internal/types/int128_types.hpp"



namespace ccm::rt::simd::traits
{

	/**
	 * @brief List of supported SIMD instruction sets in ccmath.
	 */
	enum class SupportedSimd : char
	{
		eNONE,
		eSSE,
		eAVX,
		eNEON_A64_V8,
	};

	template <SupportedSimd SimdToCheck>
	struct has_simd
	{
		static constexpr bool supported = false;
		static constexpr SupportedSimd simd = SupportedSimd::eNONE;
	};

#ifdef CCMATH_HAS_SIMD_SSE
	template <>
	struct has_simd<SupportedSimd::eSSE>
	{
		static constexpr bool supported = true;
		static constexpr SupportedSimd simd = SupportedSimd::eSSE;
	};
#endif

#ifdef CCMATH_HAS_SIMD_AVX
	template <>
	struct has_simd<SupportedSimd::eAVX>
	{
		static constexpr bool supported = true;
		static constexpr SupportedSimd simd = SupportedSimd::eAVX;
	};
#endif

#ifdef CCMATH_HAS_SIMD_NEON
	template <>
	struct has_simd<SupportedSimd::eNEON>
	{
		static constexpr bool supported = true;
		static constexpr SupportedSimd simd = SupportedSimd::eNEON_A64_V8;
	};
#endif

	template <SupportedSimd SimdToCheck>
	inline constexpr bool has_simd_v = has_simd<SimdToCheck>::value;


} // namespace ccm::rt::simd::traits
