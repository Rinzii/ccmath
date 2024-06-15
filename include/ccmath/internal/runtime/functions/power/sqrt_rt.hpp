/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/type_support.hpp"
#include "ccmath/internal/generic/functions/power/sqrt_gen.hpp"
#include "ccmath/internal/runtime/simd/functions/sqrt.hpp"
#include "ccmath/internal/support/always_false.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/fenv/rounding_mode.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"

#include <type_traits>

namespace ccm::rt::simd_impl
{
#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT64)
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
#else
	template <typename T, std::enable_if_t<std::is_floating_point_v<T> && !std::is_same_v<T, long double>, bool> = true>
#endif
	[[nodiscard]] inline T sqrt_simd_impl(T num) noexcept;

#ifdef CCMATH_SIMD
	#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT64)
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool>>
	#else
	template <typename T, std::enable_if_t<std::is_floating_point_v<T> && !std::is_same_v<T, long double>, bool>>
	#endif
	[[nodiscard]] inline T sqrt_simd_impl(T num) noexcept
	{
		intrin::simd<T, intrin::abi::native> const num_m(num);
		intrin::simd<T, intrin::abi::native> const sqrt_m = intrin::sqrt(num_m);
		return sqrt_m.convert();
	}
#endif
} // namespace ccm::rt::simd_impl

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	T sqrt_rt(T num)
	{
#if CCM_HAS_BUILTIN(__builtin_sqrt) || defined(__builtin_sqrt) // Prefer the builtins if available.
		if constexpr (std::is_same_v<T, float>) { return __builtin_sqrtf(num); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_sqrt(num); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_sqrtl(num); }
		else { return static_cast<T>(__builtin_sqrtl(static_cast<long double>(num))); }
#elif defined(CCMATH_SIMD)
		// In the unlikely event, the rounding mode is not the default, use the runtime implementation instead.
		if (CCM_UNLIKELY(ccm::support::fenv::get_rounding_mode() != FE_TONEAREST)) { return ccm::ct::sqrt_ct<T>(num); }
	#if !defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT64) // If long double is different from double, use the generic implementation instead.
		if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) { return simd_impl::sqrt_simd_impl(num); }
		else { return ccm::ct::sqrt_ct<T>(num); }
	#else										   // If long double is the same as double we can use the SIMD implementation instead.
		if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) { return simd_impl::sqrt_simd_impl(num); }
		else if constexpr (std::is_same_v<T, long double>) { return static_cast<long double>(simd_impl::sqrt_simd_impl(static_cast<double>(num))); }
		else { return ccm::gen::sqrt_gen<T>(num); }
	#endif
#else // If we don't have a builtin or SIMD, use the generic implementation.
		return ccm::gen::sqrt_gen<T>(num);
#endif
	}
} // namespace ccm::rt
