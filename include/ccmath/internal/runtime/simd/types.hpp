/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/type_support.hpp"
#include "ccmath/internal/runtime/simd/common.hpp"
#include "ccmath/internal/runtime/simd/has_simd.hpp"
#include "ccmath/internal/runtime/simd/intrin_include.hpp"
#include "ccmath/internal/types/int128_types.hpp"

#include <array>
#include <cstdint>

namespace ccm::rt::simd
{

	template <typename T>
	struct simd_types
	{

#if defined(CCMATH_HAS_SIMD_AVX)
		using generic_float_type  = __m256;
		using generic_double_type = __m256d;
	#ifdef CCM_TYPES_LONG_DOUBLE_IS_FLOAT64
		using generic_long_double_type = __m128d;
	#else
		using generic_long_double_type = void;
	#endif
		using generic_int32_type = __m256i;
		using generic_int64_type = __m256i;

#elif defined(CCMATH_HAS_SIMD_SSE2)
		using generic_float_type  = __m128;
		using generic_double_type = __m128d;
	#ifdef CCM_TYPES_LONG_DOUBLE_IS_FLOAT64
		using generic_long_double_type = __m128d;
	#else
		using generic_long_double_type = void;
	#endif
		using generic_int32_type = __m128i;
		using generic_int64_type = __m128i;

#elif defined(CCMATH_HAS_SIMD_NEON)
		using generic_float_type  = float32_t;
		using generic_double_type = float64_t;
	#ifdef CCM_TYPES_LONG_DOUBLE_IS_FLOAT64
		using generic_long_double_type = __m128d;
	#else
		using generic_long_double_type = void;
	#endif
		using generic_int32_type = int32_t;
		using generic_int64_type = int64_t;
#else
		using generic_float_type	   = void;
		using generic_double_type	   = void;
		using generic_long_double_type = void;
		using generic_int32_type	   = void;
		using generic_int64_type	   = void;
#endif

		// SSE(1)
#ifdef CCMATH_HAS_SIMD_SSE2
		using m128_type	 = __m128;
		using m128i_type = __m128i;
		using m128d_type = __m128d;
#else
		using m128_type	 = void;
		using m128i_type = void;
		using m128d_type = void;
#endif

		// AVX(1)
#ifdef CCMATH_HAS_SIMD_AVX
		using m256_type	 = __m256;
		using m256i_type = __m256i;
		using m256d_type = __m256d;
#else
		using m256_type	 = void;
		using m256i_type = void;
		using m256d_type = void;
#endif

		// NEON (ARM A64 V8)
#ifdef CCMATH_HAS_SIMD_NEON
		// Float Types
		using float32_t_type = float32_t;

		using float32x2_t_type	 = float32x2_t;
		using float32x2x2_t_type = float32x2x2_t;
		using float32x2x3_t_type = float32x2x3_t;

		using float32x4_t_type	 = float32x4_t;
		using float32x4x2_t_type = float32x4x2_t;
		using float32x4x3_t_type = float32x4x3_t;
		using float32x4x4_t_type = float32x4x4_t;

		// Double Types
		using float64_t_type = float64_t;

		using float64x1_t_type	 = float64x1_t;
		using float64x1x2_t_type = float64x1x2_t;
		using float64x1x3_t_type = float64x1x3_t;
		using float64x1x4_t_type = float64x1x4_t;

		using float64x2_t_type	 = float64x2_t;
		using float64x2x2_t_type = float64x2x2_t;
		using float64x2x3_t_type = float64x2x3_t;
		using float64x2x4_t_type = float64x2x4_t;
#else
		// Float Types
		using float32_t_type = void;

		using float32x2_t_type	 = void;
		using float32x2x2_t_type = void;
		using float32x2x3_t_type = void;

		using float32x4_t_type	 = void;
		using float32x4x2_t_type = void;
		using float32x4x3_t_type = void;
		using float32x4x4_t_type = void;

		// Double Types
		using float64_t_type = void;

		using float64x1_t_type	 = void;
		using float64x1x2_t_type = void;
		using float64x1x3_t_type = void;
		using float64x1x4_t_type = void;

		using float64x2_t_type	 = void;
		using float64x2x2_t_type = void;
		using float64x2x3_t_type = void;
		using float64x2x4_t_type = void;
#endif

		[[nodiscard]] static constexpr bool is_supported_type()
		{
			if constexpr (std::is_same_v<T, void>) { return false; }

			if constexpr (std::is_same_v<T, m128_type> || std::is_same_v<T, m128i_type> || std::is_same_v<T, m128d_type> || std::is_same_v<T, m256_type> ||
						  std::is_same_v<T, m256i_type> || std::is_same_v<T, m256d_type> || std::is_same_v<T, float32_t_type> ||
						  std::is_same_v<T, float32x2_t_type> || std::is_same_v<T, float32x2x2_t_type> || std::is_same_v<T, float32x2x3_t_type> ||
						  std::is_same_v<T, float32x4_t_type> || std::is_same_v<T, float32x4x2_t_type> || std::is_same_v<T, float32x4x3_t_type> ||
						  std::is_same_v<T, float32x4x4_t_type> || std::is_same_v<T, float64_t_type> || std::is_same_v<T, float64x1_t_type> ||
						  std::is_same_v<T, float64x1x2_t_type> || std::is_same_v<T, float64x1x3_t_type> || std::is_same_v<T, float64x1x4_t_type> ||
						  std::is_same_v<T, float64x2_t_type> || std::is_same_v<T, float64x2x2_t_type> || std::is_same_v<T, float64x2x3_t_type> ||
						  std::is_same_v<T, float64x2x4_t_type> || std::is_same_v<T, generic_float_type> || std::is_same_v<T, generic_int32_type> ||
						  std::is_same_v<T, generic_int64_type> || std::is_same_v<T, generic_double_type>)
			{
				return true;
			}

			return false;
		}
	};

	using simd_float_t  = typename simd_types<float>::generic_float_type;
	using simd_double_t = typename simd_types<double>::generic_double_type;
	using simd_long_double_t = typename simd_types<long double>::generic_long_double_type;

} // namespace ccm::rt::simd
