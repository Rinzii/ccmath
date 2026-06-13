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
#include "ccmath/internal/math/runtime/pp/simd_mask.hpp"
#include "ccmath/internal/math/runtime/pp/utility.hpp"
#include "ccmath/internal/math/runtime/pp/vec_ext.hpp"
#include "ccmath/internal/predef/attributes/always_inline.hpp"

#include <cstddef>

namespace ccm::pp
{
	// [simd.mask.reductions]
	template <std::size_t B, typename Abi>
	CCM_ALWAYS_INLINE bool all_of(basic_simd_mask<B, Abi> const & mask)
	{ return SimdTraits<detail::mask_integer_from<B>, Abi>::all_of(mask.get()); }
	template <std::size_t B, typename Abi>
	CCM_ALWAYS_INLINE bool any_of(basic_simd_mask<B, Abi> const & mask)
	{ return SimdTraits<detail::mask_integer_from<B>, Abi>::any_of(mask.get()); }
	template <std::size_t B, typename Abi>
	CCM_ALWAYS_INLINE bool none_of(basic_simd_mask<B, Abi> const & mask)
	{ return !SimdTraits<detail::mask_integer_from<B>, Abi>::any_of(mask.get()); }
	template <std::size_t B, typename Abi>
	CCM_ALWAYS_INLINE detail::SimdSizeType reduce_count(basic_simd_mask<B, Abi> const & mask)
	{ return SimdTraits<detail::mask_integer_from<B>, Abi>::popcount(mask.get()); }

	// Index of the first / last set lane. Precondition: any_of(mask) is true;
	// when no lane is set these return mask.size() as a defensive sentinel.
	template <std::size_t B, typename Abi>
	CCM_ALWAYS_INLINE detail::SimdSizeType reduce_min_index(basic_simd_mask<B, Abi> const & mask)
	{
		for (detail::SimdSizeType i = 0; i < mask.size(); ++i)
		{
			if (mask[i]) { return i; }
		}
		return mask.size();
	}
	template <std::size_t B, typename Abi>
	CCM_ALWAYS_INLINE detail::SimdSizeType reduce_max_index(basic_simd_mask<B, Abi> const & mask)
	{
		for (detail::SimdSizeType i = mask.size() - 1; i >= 0; --i)
		{
			if (mask[i]) { return i; }
		}
		return mask.size();
	}

	// Single-bool overloads (a bool models a 1-lane mask).
	CCM_ALWAYS_INLINE bool all_of(bool x)
	{ return x; }
	CCM_ALWAYS_INLINE bool any_of(bool x)
	{ return x; }
	CCM_ALWAYS_INLINE bool none_of(bool x)
	{ return !x; }
	CCM_ALWAYS_INLINE detail::SimdSizeType reduce_count(bool x)
	{ return x ? 1 : 0; }
} // namespace ccm::pp
