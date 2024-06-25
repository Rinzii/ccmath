/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/generic/functions/power/sqrt_gen.hpp"
#include "ccmath/internal/runtime/simd/simd.hpp"

namespace ccm::intrin
{
	template <class T>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE simd<T, abi::scalar> sqrt(simd<T, abi::scalar> const& a)
	{
		return simd<T, abi::scalar>(ccm::gen::sqrt_gen(a.get()));
	}
} // namespace ccm::intrin