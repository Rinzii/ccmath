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

#include "ccmath/internal/math/runtime/simd/simd.hpp"

#ifdef CCMATH_HAS_SIMD
	#ifdef CCMATH_HAS_SIMD_SSSE3
		#include "ccmath/internal/config/platform/linux.hpp"

		#if defined(CCM_TARGET_PLATFORM_LINUX)
			#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"
		#endif

namespace ccm::intrin
{
	CCM_ALWAYS_INLINE simd<float, abi::ssse3> pow(simd<float, abi::ssse3> const & a, simd<float, abi::ssse3> const & b)
	{
		// NOLINTNEXTLINE(modernize-return-braced-init-list)
		// _mm_pow_ps is a part of SVML which is a part of intel's DPC++ compiler
		// It appears Windows and macOS have SVML out the box so we only care about linux.
		#if !defined(CCM_TARGET_PLATFORM_LINUX)
		return simd<float, abi::ssse3>(_mm_pow_ps(a.get(), b.get()));
		#else
		// TODO: Replace this with a refined solution. For the time being this is temporary.
		return simd<float, abi::ssse3>(gen::pow_gen(a.convert(), b.convert()));
		#endif
	}

	CCM_ALWAYS_INLINE simd<double, abi::ssse3> pow(simd<double, abi::ssse3> const & a, simd<double, abi::ssse3> const & b)
	{
		// NOLINTNEXTLINE(modernize-return-braced-init-list)
		// _mm_pow_pd is a part of SVML which is a part of intel's DPC++ compiler
		// It appears Windows and macOS have SVML out the box so we only care about linux.
		#if !defined(CCM_TARGET_PLATFORM_LINUX)
		return simd<double, abi::ssse3>(_mm_pow_pd(a.get(), b.get()));
		#else
		// TODO: Replace this with a refined solution. For the time being this is temporary.
		return simd<double, abi::ssse3>(gen::pow_gen(a.convert(), b.convert()));
		#endif
	}
} // namespace ccm::intrin

	#endif // CCMATH_HAS_SIMD_SSSE3
#endif	   // CCMATH_HAS_SIMD
