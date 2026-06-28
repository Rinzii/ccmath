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

#include "ccmath/internal/math/generic/func/power/sqrt_gen.hpp"
#include "ccmath/internal/math/runtime/simd/func/impl/scalar/map.hpp"
#include "ccmath/internal/math/runtime/simd/simd.hpp"

#ifdef CCMATH_HAS_SIMD
	#ifdef CCMATH_HAS_SIMD_ENABLE_VECTOR_SIZE

namespace ccm::intrin
{
	template <class T, int N> CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE simd<T, abi::vector_size<N>> sqrt(simd<T, abi::vector_size<N>> const & a)
	{
		return map_scalar(a, [](T value) { return ccm::gen::sqrt_gen(value); });
	}
} // namespace ccm::intrin

	#endif
#endif
