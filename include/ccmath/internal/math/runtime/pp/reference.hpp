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
#include "ccmath/internal/math/runtime/pp/utility.hpp"
#include "ccmath/internal/predef/attributes/always_inline.hpp"

// Mutable lane proxy returned by basic_simd::operator[]. This is an extension
// over the C++26 surface (which exposes read-only element access) kept for
// ergonomics; it mirrors libc++'s reference.h.

namespace ccm::pp::detail
{
	template <typename T, typename Abi>
	class SimdReference // NOLINT(cppcoreguidelines-special-member-functions)
	{
		using Traits = SimdTraits<T, Abi>;
		typename Traits::SimdMember *data_;
		SimdSizeType index_;

	public:
		CCM_ALWAYS_INLINE SimdReference(typename Traits::SimdMember *data, SimdSizeType index) : data_(data), index_(index) {}

		SimdReference(SimdReference const &) = default;

		CCM_ALWAYS_INLINE operator T() const { return Traits::get(*data_, index_); } // NOLINT(google-explicit-constructor)

		CCM_ALWAYS_INLINE SimdReference &operator=(T value)
		{
			Traits::set(*data_, index_, value);
			return *this;
		}
	};
} // namespace ccm::pp::detail
