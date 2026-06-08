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

namespace ccm::intrin
{
	template <class T>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE simd<T, abi::scalar> sqrt(simd<T, abi::scalar> const & a)
	{ return simd<T, abi::scalar>(ccm::gen::sqrt_gen(a.get())); }

	template <class T, class Abi>
	CCM_ALWAYS_INLINE std::enable_if_t<!std::is_same_v<Abi, abi::scalar>, simd<T, Abi>> sqrt(simd<T, Abi> const & a)
	{
		return map_scalar(a, [](T value) { return ccm::gen::sqrt_gen<T>(value); });
	}
} // namespace ccm::intrin
