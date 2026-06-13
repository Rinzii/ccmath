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

#include "ccmath/internal/math/runtime/pp/declaration.hpp"
#include "ccmath/internal/math/runtime/pp/scalar.hpp"
#include "ccmath/internal/math/runtime/pp/simd.hpp"
#include "ccmath/internal/math/runtime/pp/vec_ext.hpp"
#include "ccmath/internal/predef/attributes/always_inline.hpp"

#include <cstddef>

namespace ccm::pp
{
	// Value conversion of every lane to U, preserving lane count (and thus ABI).
	// e.g. static_simd_cast<int>(basic_simd<float>) truncates toward zero per lane.
	template <typename U, typename T, typename Abi>
	CCM_ALWAYS_INLINE basic_simd<U, Abi> static_simd_cast(basic_simd<T, Abi> const & v)
	{ return basic_simd<U, Abi>::from_member(SimdTraits<T, Abi>::template convert<U>(v.get())); }

	// Bit-preserving reinterpret of each lane to a same-sized type U
	// (e.g. float <-> int32, double <-> int64). The bit pattern is unchanged.
	template <typename U, typename T, typename Abi>
	CCM_ALWAYS_INLINE basic_simd<U, Abi> simd_bit_cast(basic_simd<T, Abi> const & v)
	{
		static_assert(sizeof(U) == sizeof(T), "simd_bit_cast requires equal element size");
		return basic_simd<U, Abi>::from_member(SimdTraits<T, Abi>::template bitcast<U>(v.get()));
	}
} // namespace ccm::pp
