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

#include "ccmath/internal/math/generic/builtins/power/pow.hpp"
#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"
#include "ccmath/internal/math/generic/func/power/pow_impl/pow_simd_impl.hpp"
#include "ccmath/internal/math/generic/func/power/pow_impl/powf_simd_impl.hpp"
#include "ccmath/internal/math/runtime/pp/pp.hpp"
#include "ccmath/internal/math/runtime/simd/func/catalog.hpp"
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/fenv/rounding_mode.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"

#include <type_traits>

namespace ccm::rt::simd_impl
{
#ifdef CCM_TYPES_LONG_DOUBLE_IS_FLOAT64
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
		pp::native_simd<T> const base_v(base);
		pp::native_simd<T> const exp_v(exp);
		// Single and double precision use the dedicated vectorized kernels, which are bit
		// identical to the scalar correctly rounded powf and pow. Other types use the
		// per-lane std::simd baseline for now.
		if constexpr (std::is_same_v<T, float>) { return gen::impl::powf_simd(base_v, exp_v)[0]; }
		else if constexpr (std::is_same_v<T, double>) { return gen::impl::pow_simd(base_v, exp_v)[0]; }
		else
		{
			return pp::pow(base_v, exp_v)[0];
		}
	}
#endif
} // namespace ccm::rt::simd_impl

namespace ccm::rt
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	T pow_rt(T base, T exp)
	{
#ifndef CCM_CONFIG_TEST_DISABLE_RUNTIME_BUILTIN_POW
		if constexpr (ccm::builtin::has_runtime_pow<T>)
		{
			// The runtime builtin lowers to libm, which is not correctly rounded outside round to
			// nearest. Outside FE_TONEAREST use the correctly-rounded generic kernel instead.
			if (CCM_UNLIKELY(ccm::support::fenv::get_rounding_mode() != FE_TONEAREST)) { return gen::pow_gen<T>(base, exp); }
			return ccm::builtin::pow_rt(base, exp);
		}
#endif
#ifdef CCMATH_HAS_SIMD
		// In the unlikely event, the rounding mode is not the default, use the runtime implementation instead.
		if (CCM_UNLIKELY(ccm::support::fenv::get_rounding_mode() != FE_TONEAREST)) { return gen::pow_gen<T>(base, exp); }
	#if !defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT64) // If long double is different from double, use the generic implementation instead.
		if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) { return simd_impl::pow_simd_impl(base, exp); }
		else
		{
			return gen::pow_gen<T>(base, exp);
		}
	#else // If long double is the same as double, we can use the SIMD implementation instead.
		if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) { return simd_impl::pow_simd_impl(base, exp); }
		else if constexpr (std::is_same_v<T, long double>)
		{
			return static_cast<long double>(simd_impl::pow_simd_impl(static_cast<double>(base), static_cast<double>(exp)));
		}
		else
		{
			return ccm::gen::pow_gen<T>(base, exp);
		}
	#endif
#else // If we don't have a builtin or SIMD, use the generic implementation.
		return gen::pow_gen<T>(base, exp);
#endif
	}
} // namespace ccm::rt
