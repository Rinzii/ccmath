/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/type_support.hpp"
#include "ccmath/internal/runtime/simd/intrin_include.hpp"
#include "ccmath/internal/runtime/simd/types.hpp"
#include "ccmath/internal/support/always_false.hpp"

#include <type_traits>

namespace ccm::rt::simd
{

	[[nodiscard]] inline simd_float_t mm_sqrt_ss(simd_float_t num) noexcept
	{
#ifdef CCMATH_HAS_SIMD
	#if defined(CCMATH_HAS_SIMD_AVX)
		return _mm256_sqrt_ps(num);
	#elif defined(CCMATH_HAS_SIMD_SSE2)
		return _mm_sqrt_ss(num);
	#elif defined(CCMATH_HAS_SIMD_NEON)
		return vrsqrteq_f32(num);
	#else
		assert(false); // This should never be reached.
		return num;
	#endif
#else
		assert(false); // This should never be reached.
		return num;
#endif
	}

	[[nodiscard]] inline simd_double_t mm_sqrt_ss(simd_double_t num) noexcept
	{
#ifdef CCMATH_HAS_SIMD
	#if defined(CCMATH_HAS_SIMD_AVX)
		return _mm256_sqrt_pd(num);
	#elif defined(CCMATH_HAS_SIMD_SSE2)
		return _mm_sqrt_sd(num, num);
	#elif defined(CCMATH_HAS_SIMD_NEON)
		return vrsqrteq_f64(num);
	#else
		assert(false); // This should never be reached.
		return num;
	#endif
#else
		assert(false); // This should never be reached.
		return num;
#endif
	}

#ifdef CCM_TYPES_LONG_DOUBLE_IS_FLOAT64
	[[nodiscard]] inline simd_long_double_t mm_sqrt_ss(simd_long_double_t num) noexcept
	{
	#ifdef CCMATH_HAS_SIMD
		#if defined(CCMATH_HAS_SIMD_AVX)
		return _mm256_sqrt_pd(num);
		#elif defined(CCMATH_HAS_SIMD_SSE2)
		return _mm_sqrt_sd(num, num);
		#elif defined(CCMATH_HAS_SIMD_NEON)
		return vrsqrteq_f64(num);
		#else
		assert(false); // This should never be reached.
		return num;
		#endif
	#else
		assert(false); // This should never be reached.
		return num;
	#endif
	}
#endif
} // namespace ccm::rt::simd