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
#include "ccmath/internal/math/runtime/pp/simd.hpp"
#include "ccmath/internal/math/runtime/pp/simd_mask.hpp"
#include "ccmath/internal/math/runtime/pp/traits.hpp"
#include "ccmath/internal/predef/attributes/always_inline.hpp"

#include <array>
#include <cstddef>
#include <type_traits>

// Concatenate several simds into a wider one, or split a wide simd into equal
// pieces. Under the vector_size ABI the combined / piece widths must be
// representable (a power of two, or 1 for scalar).

namespace ccm::pp
{
	template <typename T, typename... Abis>
	CCM_ALWAYS_INLINE basic_simd<T, detail::deduce_t<T, (basic_simd<T, Abis>::size() + ...)>> simd_cat(basic_simd<T, Abis> const &... xs)
	{
		using R = basic_simd<T, detail::deduce_t<T, (basic_simd<T, Abis>::size() + ...)>>;
		R r;
		detail::SimdSizeType off = 0;
		(
			[&] {
				for (detail::SimdSizeType i = 0; i < xs.size(); ++i)
				{
					r[off + i] = xs[i];
				}
				off += xs.size();
			}(),
			...);
		return r;
	}

	template <typename V, typename T, typename Abi, std::enable_if_t<std::is_same_v<typename V::value_type, T>, int> = 0>
	CCM_ALWAYS_INLINE std::array<V, static_cast<std::size_t>(basic_simd<T, Abi>::size() / V::size())> simd_split(basic_simd<T, Abi> const & v)
	{
		constexpr detail::SimdSizeType parts = basic_simd<T, Abi>::size() / V::size();
		std::array<V, static_cast<std::size_t>(parts)> out;
		detail::SimdSizeType off = 0;
		for (detail::SimdSizeType p = 0; p < parts; ++p)
		{
			for (detail::SimdSizeType i = 0; i < V::size(); ++i)
			{
				out[static_cast<std::size_t>(p)][i] = v[off + i];
			}
			off += V::size();
		}
		return out;
	}

	template <std::size_t B, typename... Abis>
	CCM_ALWAYS_INLINE basic_simd_mask<B, detail::deduce_t<detail::mask_integer_from<B>, (basic_simd_mask<B, Abis>::size() + ...)>>
	simd_cat(basic_simd_mask<B, Abis> const &... xs)
	{
		using R					 = basic_simd_mask<B, detail::deduce_t<detail::mask_integer_from<B>, (basic_simd_mask<B, Abis>::size() + ...)>>;
		unsigned long long bits	 = 0;
		detail::SimdSizeType off = 0;
		(
			[&] {
				bits |= (xs.to_ullong() << off);
				off += xs.size();
			}(),
			...);
		return R::from_bits(bits);
	}

	template <typename MaskV, std::size_t B, typename Abi, std::enable_if_t<is_simd_mask_v<MaskV>, int> = 0>
	CCM_ALWAYS_INLINE std::array<MaskV, static_cast<std::size_t>(basic_simd_mask<B, Abi>::size() / MaskV::size())> simd_split(basic_simd_mask<B, Abi> const & m)
	{
		constexpr detail::SimdSizeType parts = basic_simd_mask<B, Abi>::size() / MaskV::size();
		std::array<MaskV, static_cast<std::size_t>(parts)> out;
		unsigned long long const bits = m.to_ullong();
		detail::SimdSizeType off	  = 0;
		for (detail::SimdSizeType p = 0; p < parts; ++p)
		{
			out[static_cast<std::size_t>(p)] = MaskV::from_bits(bits >> off);
			off += MaskV::size();
		}
		return out;
	}
} // namespace ccm::pp
