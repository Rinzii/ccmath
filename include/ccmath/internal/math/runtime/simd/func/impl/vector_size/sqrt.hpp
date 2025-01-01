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
	#ifdef CCMATH_HAS_SIMD_ENABLE_VECTOR_SIZE

namespace ccm::intrin
{

	template <class T, int N>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE simd<T, abi::vector_size<N>> sqrt(simd<T, abi::vector_size<N>> const & a)
	{
		simd<T, abi::vector_size<N>> result;

		// TODO: Implement a runtime vector_size<N> sqrt that is optimized for runtime.
		// CCM_SIMD_VECTORIZE for (int i = 0; i < a.size(); ++i)
		//{
		//	result.get()[i] = sqrt(a[i]);
		//}

		return result;
	}

} // namespace ccm::intrin

	#endif // CCMATH_HAS_SIMD_ENABLE_VECTOR_SIZE
#endif	   // CCMATH_HAS_SIMD
