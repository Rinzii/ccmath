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

	[[nodiscard]] inline simd_float_t mm_set_ss(float num) noexcept;
	[[nodiscard]] inline simd_double_t mm_set_ss(double num) noexcept;
#ifdef CCM_TYPES_LONG_DOUBLE_IS_FLOAT64
	[[nodiscard]] inline simd_long_double_t mm_set_ss(long double num) noexcept;
#endif

#if defined(CCMATH_HAS_SIMD)

	[[nodiscard]] inline simd_float_t mm_set_ss(float num) noexcept
	{
	#if defined(CCMATH_HAS_SIMD_AVX)
		return _mm256_set1_ps(num);
	#elif defined(CCMATH_HAS_SIMD_SSE2)
		return _mm_set_ss(num);
	#else
		return num; // This should never be reached.
	#endif
	}

	[[nodiscard]] inline simd_double_t mm_set_ss(double num) noexcept
	{
	#if defined(CCMATH_HAS_SIMD_AVX)
		return _mm256_set1_pd(num);
	#elif defined(CCMATH_HAS_SIMD_SSE2)
		return _mm_set_sd(num);
	#else
		return num; // This should never be reached.
	#endif
	}

	#ifdef CCM_TYPES_LONG_DOUBLE_IS_FLOAT64
	[[nodiscard]] inline simd_long_double_t mm_set_ss(long double num) noexcept
	{
		#if defined(CCMATH_HAS_SIMD_AVX)
		return _mm256_set1_pd(static_assert<double>(num));
		#elif defined(CCMATH_HAS_SIMD_SSE2)
		return _mm_set_sd(static_assert<double>(num));
		#else
		return num; // This should never be reached.
		#endif
	}
	#endif

#endif

} // namespace ccm::rt::simd
