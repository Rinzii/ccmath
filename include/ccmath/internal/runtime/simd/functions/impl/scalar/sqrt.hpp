/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/runtime/simd/simd.hpp"

namespace ccm::intrin
{
	template <class T>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd<T, abi::scalar> sqrt(simd<T, abi::scalar> const& a)
	{
		// TODO: Implement a runtime scalar sqrt that is optimized for runtime.
		return simd<T, abi::scalar>(a.get());
	}
} // namespace ccm::intrin