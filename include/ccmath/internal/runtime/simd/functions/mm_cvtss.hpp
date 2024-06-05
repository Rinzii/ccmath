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
#include "ccmath/internal/support/unreachable.hpp"

#include <type_traits>

namespace ccm::rt::simd
{
	[[nodiscard]] inline float mm_cvtss(simd_float_t num) noexcept;
	[[nodiscard]] inline double mm_cvtss(simd_double_t num) noexcept;
#ifdef CCM_TYPES_LONG_DOUBLE_IS_FLOAT64
	[[nodiscard]] inline double mm_cvtss(simd_long_double_t num) noexcept;
#endif

#if defined(CCMATH_HAS_SIMD)
	[[nodiscard]] inline float mm_cvtss(simd_float_t num) noexcept
	{
	#if defined(CCMATH_HAS_SIMD_AVX)
		return _mm256_cvtss_f32(num);
	#elif defined(CCMATH_HAS_SIMD_SSE2)
		return _mm_cvtss_f32(num);
	#else
		ccm::support::unreachable(); // This should never be reached.
		return num;
	#endif
	}

	[[nodiscard]] inline double mm_cvtss(simd_double_t num) noexcept
	{
	#if defined(CCMATH_HAS_SIMD_AVX)
		return _mm256_cvtsd_f64(num);
	#elif defined(CCMATH_HAS_SIMD_SSE2)
		return _mm_cvtsd_f64(num);
	#else
		ccm::support::unreachable(); // This should never be reached.
		return num;
	#endif
	}

	#ifdef CCM_TYPES_LONG_DOUBLE_IS_FLOAT64
	[[nodiscard]] inline double mm_cvtss(simd_long_double_t num) noexcept
	{
		#if defined(CCMATH_HAS_SIMD_AVX)
		return _mm256_cvtsd_f64(static_assert<simd_long_double_t>(num));
		#elif defined(CCMATH_HAS_SIMD_SSE2)
		return _mm_cvtsd_f64(static_assert<simd_long_double_t>(num));
		#else
		ccm::support::unreachable(); // This should never be reached.
		return num;
		#endif
	}
	#endif

#endif

} // namespace ccm::rt::simd
