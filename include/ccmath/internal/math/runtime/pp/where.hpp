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
#include "ccmath/internal/math/runtime/pp/utility.hpp"
#include "ccmath/internal/math/runtime/pp/vec_ext.hpp"
#include "ccmath/internal/predef/attributes/always_inline.hpp"

#include <cstddef>

namespace ccm::pp
{
	// Lane-wise blend: pick a where mask is set, else b.
	template <typename T, typename Abi>
	CCM_ALWAYS_INLINE basic_simd<T, Abi> simd_select(basic_simd_mask<sizeof(T), Abi> const & mask, basic_simd<T, Abi> const & a, basic_simd<T, Abi> const & b)
	{
		return basic_simd<T, Abi>::from_member(SimdTraits<T, Abi>::select(mask.get(), a.get(), b.get()));
	}

	// [simd.whereexpr] Masked, write-only view onto a simd object.
	template <typename MaskType, typename SimdType> class where_expression
	{
		MaskType mask_;
		SimdType * data_;

	public:
		using value_type = typename SimdType::value_type;

		CCM_ALWAYS_INLINE where_expression(MaskType const & mask, SimdType & data) : mask_(mask), data_(&data) {}

		// NOLINTBEGIN(cppcoreguidelines-c-copy-assignment-signature,misc-unconventional-assign-operator)
		CCM_ALWAYS_INLINE void operator=(SimdType const & rhs)
		{
			*data_ = simd_select(mask_, rhs, *data_);
		}
		CCM_ALWAYS_INLINE void operator=(value_type rhs)
		{
			*data_ = simd_select(mask_, SimdType(rhs), *data_);
		}
		// NOLINTEND(cppcoreguidelines-c-copy-assignment-signature,misc-unconventional-assign-operator)

		CCM_ALWAYS_INLINE void operator+=(SimdType const & rhs)
		{
			*data_ = simd_select(mask_, *data_ + rhs, *data_);
		}
		CCM_ALWAYS_INLINE void operator-=(SimdType const & rhs)
		{
			*data_ = simd_select(mask_, *data_ - rhs, *data_);
		}
		CCM_ALWAYS_INLINE void operator*=(SimdType const & rhs)
		{
			*data_ = simd_select(mask_, *data_ * rhs, *data_);
		}
		CCM_ALWAYS_INLINE void operator/=(SimdType const & rhs)
		{
			*data_ = simd_select(mask_, *data_ / rhs, *data_);
		}

		// Masked load: read only the selected lanes from memory, leave the rest.
		template <typename Flags = simd_flags<>> CCM_ALWAYS_INLINE void copy_from(value_type const * ptr, Flags /*flags*/ = {})
		{
			for (detail::SimdSizeType i = 0; i < SimdType::size(); ++i)
			{
				if (mask_[i])
				{
					(*data_)[i] = ptr[i];
				}
			}
		}
		// Masked store: write only the selected lanes to memory, leave the rest.
		template <typename Flags = simd_flags<>> CCM_ALWAYS_INLINE void copy_to(value_type * ptr, Flags /*flags*/ = {}) const
		{
			for (detail::SimdSizeType i = 0; i < SimdType::size(); ++i)
			{
				if (mask_[i])
				{
					ptr[i] = (*data_)[i];
				}
			}
		}
	};

	// [simd.whereexpr] Masked, read-only view onto a const simd object.
	template <typename MaskType, typename SimdType> class const_where_expression
	{
		MaskType mask_;
		SimdType const * data_;

	public:
		using value_type = typename SimdType::value_type;

		CCM_ALWAYS_INLINE const_where_expression(MaskType const & mask, SimdType const & data) : mask_(mask), data_(&data) {}

		// Masked store: write only the selected lanes to memory.
		template <typename Flags = simd_flags<>> CCM_ALWAYS_INLINE void copy_to(value_type * ptr, Flags /*flags*/ = {}) const
		{
			for (detail::SimdSizeType i = 0; i < SimdType::size(); ++i)
			{
				if (mask_[i])
				{
					ptr[i] = (*data_)[i];
				}
			}
		}
	};

	template <typename T, typename Abi>
	CCM_ALWAYS_INLINE where_expression<basic_simd_mask<sizeof(T), Abi>, basic_simd<T, Abi>> where(basic_simd_mask<sizeof(T), Abi> const & mask,
																								  basic_simd<T, Abi> & value)
	{
		return { mask, value };
	}

	template <typename T, typename Abi>
	CCM_ALWAYS_INLINE const_where_expression<basic_simd_mask<sizeof(T), Abi>, basic_simd<T, Abi>> where(basic_simd_mask<sizeof(T), Abi> const & mask,
																										basic_simd<T, Abi> const & value)
	{
		return { mask, value };
	}
} // namespace ccm::pp
