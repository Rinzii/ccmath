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

#include "ccmath/internal/config/freestanding.hpp"
#include "ccmath/internal/math/runtime/pp/declaration.hpp"
#include "ccmath/internal/math/runtime/pp/scalar.hpp"
#include "ccmath/internal/math/runtime/pp/utility.hpp"
#include "ccmath/internal/math/runtime/pp/vec_ext.hpp"
#include "ccmath/internal/predef/attributes/always_inline.hpp"

#include <cstddef>

// <bitset> is not a freestanding header, so to_bitset() is only available in hosted builds.
#if !defined(CCM_CONFIG_FREESTANDING)
	#include <bitset>
#endif

namespace ccm::pp
{
	// [simd.mask] A data-parallel boolean. Parametrized by lane byte width to
	// match the C++26 basic_simd_mask, internally backed by the signed integer of
	// that width through the shared SimdTraits.
	template <std::size_t Bytes, typename Abi> class basic_simd_mask
	{
		using IntT		  = detail::mask_integer_from<Bytes>;
		using Traits	  = SimdTraits<IntT, Abi>;
		using member_type = typename Traits::MaskMember;

		member_type data_;

	public:
		using value_type = bool;
		using abi_type	 = Abi;

		[[nodiscard]] static constexpr detail::SimdSizeType size()
		{
			return Traits::size;
		}

		basic_simd_mask() = default;
		CCM_ALWAYS_INLINE explicit basic_simd_mask(bool value) : data_(Traits::mbroadcast(value)) {}

		CCM_ALWAYS_INLINE static basic_simd_mask from_member(member_type const & m)
		{
			basic_simd_mask r;
			r.data_ = m;
			return r;
		}
		[[nodiscard]] CCM_ALWAYS_INLINE member_type const & get() const
		{
			return data_;
		}

		CCM_ALWAYS_INLINE bool operator[](detail::SimdSizeType i) const
		{
			return Traits::mget(data_, i);
		}

		// Bit serialization: lane i maps to bit i (lane 0 is the least significant).
		[[nodiscard]] CCM_ALWAYS_INLINE unsigned long long to_ullong() const
		{
			unsigned long long bits = 0;
			for (detail::SimdSizeType i = 0; i < size(); ++i)
			{
				if (Traits::mget(data_, i))
				{
					bits |= (1ULL << i);
				}
			}
			return bits;
		}
#if !defined(CCM_CONFIG_FREESTANDING)
		[[nodiscard]] CCM_ALWAYS_INLINE std::bitset<static_cast<std::size_t>(SimdTraits<detail::mask_integer_from<Bytes>, Abi>::size)> to_bitset() const
		{
			return std::bitset<static_cast<std::size_t>(size())>(to_ullong());
		}
#endif
		CCM_ALWAYS_INLINE static basic_simd_mask from_bits(unsigned long long bits)
		{
			basic_simd_mask r;
			for (detail::SimdSizeType i = 0; i < size(); ++i)
			{
				Traits::mset(r.data_, i, ((bits >> i) & 1ULL) != 0);
			}
			return r;
		}

		// Bitwise / logical combinators.
		CCM_ALWAYS_INLINE friend basic_simd_mask operator&(basic_simd_mask const & a, basic_simd_mask const & b)
		{
			return from_member(Traits::mand(a.data_, b.data_));
		}
		CCM_ALWAYS_INLINE friend basic_simd_mask operator|(basic_simd_mask const & a, basic_simd_mask const & b)
		{
			return from_member(Traits::mor(a.data_, b.data_));
		}
		CCM_ALWAYS_INLINE friend basic_simd_mask operator^(basic_simd_mask const & a, basic_simd_mask const & b)
		{
			return from_member(Traits::mxor(a.data_, b.data_));
		}
		CCM_ALWAYS_INLINE friend basic_simd_mask operator&&(basic_simd_mask const & a, basic_simd_mask const & b)
		{
			return from_member(Traits::mand(a.data_, b.data_));
		}
		CCM_ALWAYS_INLINE friend basic_simd_mask operator||(basic_simd_mask const & a, basic_simd_mask const & b)
		{
			return from_member(Traits::mor(a.data_, b.data_));
		}
		CCM_ALWAYS_INLINE basic_simd_mask operator!() const
		{
			return from_member(Traits::mnot(data_));
		}
		CCM_ALWAYS_INLINE basic_simd_mask operator~() const
		{
			return from_member(Traits::mnot(data_));
		}

		CCM_ALWAYS_INLINE friend basic_simd_mask operator==(basic_simd_mask const & a, basic_simd_mask const & b)
		{
			return from_member(Traits::mnot(Traits::mxor(a.data_, b.data_)));
		}
		CCM_ALWAYS_INLINE friend basic_simd_mask operator!=(basic_simd_mask const & a, basic_simd_mask const & b)
		{
			return from_member(Traits::mxor(a.data_, b.data_));
		}
	};
} // namespace ccm::pp
