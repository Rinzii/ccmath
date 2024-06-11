/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/type_support.hpp"
#include "ccmath/internal/runtime/simd/functions/mm_cvtss.hpp"
#include "ccmath/internal/runtime/simd/functions/mm_set_ss.hpp"
#include "ccmath/internal/runtime/simd/functions/mm_sqrt_ss.hpp"
#include "ccmath/internal/runtime/simd/intrin_include.hpp"
#include "ccmath/internal/runtime/simd/types.hpp"
#include "ccmath/internal/support/always_false.hpp"

#include <type_traits>

namespace ccm::rt::simd
{
#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT64)
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
#else
	template <typename T, std::enable_if_t<std::is_floating_point_v<T> && !std::is_same_v<T, long double>, bool> = true>
#endif
	[[nodiscard]] inline T sqrt_simd(T num) noexcept;

#ifdef CCMATH_HAS_SIMD
	#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT64)
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool>>
	#else
	template <typename T, std::enable_if_t<std::is_floating_point_v<T> && !std::is_same_v<T, long double>, bool>>
	#endif
	[[nodiscard]] inline T sqrt_simd(T num) noexcept
	{
		if constexpr (std::is_same_v<T, float>)
		{
			simd_float_t num_m		  = mm_set_ss(num);
			simd_float_t const sqrt_m = mm_sqrt_ss(num_m);
			return mm_cvtss(sqrt_m);
		}
		if constexpr (std::is_same_v<T, double>)
		{
			simd_double_t num_m		   = mm_set_ss(num);
			simd_double_t const sqrt_m = mm_sqrt_ss(num_m);
			return mm_cvtss(sqrt_m);
		}
	#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT64)
		if constexpr (std::is_same_v<T, long double>)
		{
			simd_long_double_t num_m		= mm_set_ss(static_cast<double>(num));
			simd_long_double_t const sqrt_m = mm_sqrt_ss(num_m);
			return static_cast<long double>(mm_cvtss(sqrt_m));
		}
	#else // If long double is not 64-bits, then we must use our generic sqrt function.

	simd_double_t num_m		= mm_set_ss(static_cast<double>(num));
	simd_double_t const sqrt_m = mm_sqrt_ss(num_m);
	return static_cast<T>(mm_cvtss(sqrt_m));
	#endif
	}
#endif
} // namespace ccm::rt::simd