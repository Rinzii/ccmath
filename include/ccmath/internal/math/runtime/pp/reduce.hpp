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
#include "ccmath/internal/math/runtime/pp/simd_mask.hpp"
#include "ccmath/internal/math/runtime/pp/vec_ext.hpp"
#include "ccmath/internal/math/runtime/pp/where.hpp"
#include "ccmath/internal/predef/attributes/always_inline.hpp"

#include <functional>
#include <limits>

// Horizontal reductions: collapse all lanes of a simd into a single scalar.

namespace ccm::pp
{
	namespace detail
	{
		// Neutral element for a masked min reduction (so masked-off lanes never win).
		template <typename T>
		CCM_ALWAYS_INLINE T reduce_min_identity()
		{
			if constexpr (std::is_floating_point_v<T>) { return std::numeric_limits<T>::infinity(); }
			else
			{
				return std::numeric_limits<T>::max();
			}
		}
		template <typename T>
		CCM_ALWAYS_INLINE T reduce_max_identity()
		{
			if constexpr (std::is_floating_point_v<T>) { return -std::numeric_limits<T>::infinity(); }
			else
			{
				return std::numeric_limits<T>::lowest();
			}
		}
	} // namespace detail

	// Sum of all lanes.
	template <typename T, typename Abi>
	CCM_ALWAYS_INLINE T reduce(basic_simd<T, Abi> const & v)
	{ return SimdTraits<T, Abi>::hadd(v.get()); }

	// Reduction with an arbitrary associative binary operation.
	template <typename T, typename Abi, typename BinaryOp>
	CCM_ALWAYS_INLINE T reduce(basic_simd<T, Abi> const & v, BinaryOp op)
	{
		T acc = v[0];
		for (detail::SimdSizeType i = 1; i < v.size(); ++i) { acc = op(acc, v[i]); }
		return acc;
	}

	template <typename T, typename Abi>
	CCM_ALWAYS_INLINE T reduce_min(basic_simd<T, Abi> const & v)
	{ return SimdTraits<T, Abi>::hmin(v.get()); }
	template <typename T, typename Abi>
	CCM_ALWAYS_INLINE T reduce_max(basic_simd<T, Abi> const & v)
	{ return SimdTraits<T, Abi>::hmax(v.get()); }

	// Masked reductions: masked-off lanes contribute the identity element.
	template <typename T, typename Abi, typename BinaryOp = std::plus<>>
	CCM_ALWAYS_INLINE T reduce(basic_simd<T, Abi> const & v, basic_simd_mask<sizeof(T), Abi> const & m, T identity, BinaryOp op = {})
	{
		basic_simd<T, Abi> const sel = simd_select(m, v, basic_simd<T, Abi>(identity));
		T acc						 = sel[0];
		for (detail::SimdSizeType i = 1; i < sel.size(); ++i) { acc = op(acc, sel[i]); }
		return acc;
	}
	template <typename T, typename Abi>
	CCM_ALWAYS_INLINE T reduce_min(basic_simd<T, Abi> const & v, basic_simd_mask<sizeof(T), Abi> const & m)
	{ return reduce_min(simd_select(m, v, basic_simd<T, Abi>(detail::reduce_min_identity<T>()))); }
	template <typename T, typename Abi>
	CCM_ALWAYS_INLINE T reduce_max(basic_simd<T, Abi> const & v, basic_simd_mask<sizeof(T), Abi> const & m)
	{ return reduce_max(simd_select(m, v, basic_simd<T, Abi>(detail::reduce_max_identity<T>()))); }
} // namespace ccm::pp
