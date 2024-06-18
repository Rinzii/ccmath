/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/arch/check_simd_support.hpp"

#include "ccmath/internal/runtime/simd/instructions/scalar.hpp"
#include "common.hpp"
#include "pack.hpp"
#include "vector_size.hpp"

#ifdef CCMATH_HAS_SIMD
	#ifdef CCMATH_HAS_SIMD_SSE2
		#include "instructions/sse2.hpp"
	#endif

	#ifdef CCMATH_HAS_SIMD_SSE3
		#include "instructions/sse3.hpp"
	#endif

	#ifdef CCMATH_HAS_SIMD_SSSE3
		#include "instructions/ssse3.hpp"
	#endif

	#ifdef CCMATH_HAS_SIMD_SSE4
		#include "instructions/sse4.hpp"
	#endif

	#ifdef CCMATH_HAS_SIMD_AVX
		#include "instructions/avx.hpp"
	#endif

	#ifdef CCMATH_HAS_SIMD_AVX2
		#include "instructions/avx2.hpp"
	#endif

	#ifdef CCMATH_HAS_SIMD_AVX512F
		#include "instructions/avx512.hpp"
	#endif

	#ifdef CCMATH_HAS_SIMD_NEON
		#include "instructions/neon.hpp"
	#endif
#endif

namespace ccm::intrin
{
	namespace abi
	{
#if defined(CCMATH_HAS_SIMD_AVX512F)
		using native = avx512;
#elif defined(CCMATH_HAS_SIMD_AVX2)
		using native = avx2;
#elif defined(CCMATH_HAS_SIMD_AVX)
		using native = avx;
#elif defined(CCMATH_HAS_SIMD_SSE4)
		using native = sse4;
#elif defined(CCMATH_HAS_SIMD_SSSE3)
		using native = ssse3;
#elif defined(CCMATH_HAS_SIMD_SSE3)
		using native = sse3;
#elif defined(CCMATH_HAS_SIMD_SSE2)
		using native = sse2;
#elif defined(CCMATH_HAS_SIMD_NEON) && !defined(__ARM_FEATURE_SVE_BITS) && !defined(__ARM_FEATURE_SVE)
		using native = neon;
#elif defined(CCMATH_HAS_SIMD_ENABLE_VECTOR_SIZE)
	#if defined(__ARM_FEATURE_SVE_BITS)
		using native = vector_size<__ARM_FEATURE_SVE_BITS / 8>;
	#else
		#if defined(__ARM_FEATURE_SVE)
		using native = vector_size<64>;
		#else
		using native = vector_size<32>;
		#endif
	#endif
#else
		using native = pack<8>;
#endif
	} // namespace abi

	template <class T>
	using native_simd = simd<T, abi::native>;
} // namespace ccm::intrin