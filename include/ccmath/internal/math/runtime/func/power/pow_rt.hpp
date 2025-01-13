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

#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"
#include "ccmath/internal/math/runtime/simd/func/pow.hpp"
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/fenv/rounding_mode.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"

#include <type_traits>

namespace ccm::rt::simd_impl
{
#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT64)
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
#else
	template <typename T, std::enable_if_t<std::is_floating_point_v<T> && !std::is_same_v<T, long double>, bool> = true>
#endif
	[[nodiscard]] inline T pow_simd_impl(T base, T exp) noexcept;

#ifdef CCMATH_HAS_SIMD
	#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT64)
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool>>
	#else
	template <typename T, std::enable_if_t<std::is_floating_point_v<T> && !std::is_same_v<T, long double>, bool>>
	#endif
	[[nodiscard]] inline T pow_simd_impl(T base, T exp) noexcept
	{
		intrin::simd<T, intrin::abi::native> const base_m(base);
		intrin::simd<T, intrin::abi::native> const exp_m(exp);
		intrin::simd<T, intrin::abi::native> const pow_m = intrin::pow(base_m, exp_m);
		return pow_m.convert();
	}
#endif
} // namespace ccm::rt::simd_impl

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	T pow_rt(T base, T exp)
	{
#if CCM_HAS_BUILTIN(__builtin_pow) || defined(__builtin_pow) // Prefer the builtins if available.
		if constexpr (std::is_same_v<T, float>) { return __builtin_powf(base, exp); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_pow(base, exp); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_powl(base, exp); }
		else { return static_cast<T>(__builtin_powl(static_cast<long double>(base), static_cast<long double>(exp))); }
#elif defined(CCMATH_HAS_SIMD)
		// In the unlikely event, the rounding mode is not the default, use the runtime implementation instead.
		if (CCM_UNLIKELY(ccm::support::fenv::get_rounding_mode() != FE_TONEAREST)) { return gen::pow_gen<T>(base, exp); }
	#if !defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT64) // If long double is different from double, use the generic implementation instead.
		if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) { return simd_impl::pow_simd_impl(base, exp); }
		else { return gen::pow_gen<T>(base, exp); }
	#else										   // If long double is the same as double, we can use the SIMD implementation instead.
		if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) { return simd_impl::pow_simd_impl(base, exp); }
		else if constexpr (std::is_same_v<T, long double>)
		{
			return static_cast<long double>(simd_impl::pow_simd_impl(static_cast<double>(base), static_cast<double>(exp)));
		}
		else { return ccm::gen::pow_gen<T>(base, exp); }
	#endif
#else // If we don't have a builtin or SIMD, use the generic implementation.
		return gen::pow_gen<T>(base, exp);
#endif
	}
} // namespace ccm::rt
