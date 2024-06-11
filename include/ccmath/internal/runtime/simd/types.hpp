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

#include <cstdint>
#include <type_traits>

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
		using generic_long_double_type = __m256d;
	#endif
		using generic_int32_type = __m256i;
		using generic_int64_type = __m256i;

#elif defined(CCMATH_HAS_SIMD_SSE2)
		using generic_float_type  = __m128;
		using generic_double_type = __m128d;
	#ifdef CCM_TYPES_LONG_DOUBLE_IS_FLOAT64
		using generic_long_double_type = __m128d;
	#else
		using generic_long_double_type = __m128d;
	#endif
		using generic_int32_type = __m128i;
		using generic_int64_type = __m128i;
#else
		using generic_float_type	   = float;
		using generic_double_type	   = double;
		using generic_long_double_type = long double;
		using generic_int32_type	   = std::int32_t;
		using generic_int64_type	   = std::int64_t;
#endif

		[[nodiscard]] static constexpr bool is_supported_type()
		{
#ifdef CCM_TYPES_LONG_DOUBLE_IS_FLOAT64
			return static_cast<bool>(std::is_same_v<T, generic_float_type> || std::is_same_v<T, generic_double_type> ||
									 std::is_same_v<T, generic_long_double_type> || std::is_same_v<T, generic_int32_type> ||
									 std::is_same_v<T, generic_int64_type>);
#else
			return static_cast<bool>(std::is_same_v<T, generic_float_type> || std::is_same_v<T, generic_double_type> ||
									 std::is_same_v<T, generic_int32_type> || std::is_same_v<T, generic_int64_type>);


#endif
		}
	};

	using simd_float_t  = typename simd_types<float>::generic_float_type;
	using simd_double_t = typename simd_types<double>::generic_double_type;
	using simd_long_double_t = typename simd_types<long double>::generic_long_double_type;

} // namespace ccm::rt::simd
