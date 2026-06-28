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
#include "ccmath/internal/math/runtime/simd/simd.hpp"

#ifdef CCMATH_HAS_SIMD
	#ifdef CCMATH_HAS_SIMD_NEON
namespace ccm::intrin
{
	CCM_ALWAYS_INLINE simd<float, abi::neon> pow(simd<float, abi::neon> const & a, simd<float, abi::neon> const & b)
	{
		return simd<float, abi::neon>(gen::pow_gen(a.convert(), b.convert()));
	}

	CCM_ALWAYS_INLINE simd<double, abi::neon> pow(simd<double, abi::neon> const & a, simd<double, abi::neon> const & b)
	{
		return simd<double, abi::neon>(gen::pow_gen(a.convert(), b.convert()));
	}
} // namespace ccm::intrin
	#endif
#endif
