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
#include "ccmath/internal/math/runtime/pp/flags.hpp"
#include "ccmath/internal/math/runtime/pp/reference.hpp"
#include "ccmath/internal/math/runtime/pp/scalar.hpp"
#include "ccmath/internal/math/runtime/pp/simd_mask.hpp"
#include "ccmath/internal/math/runtime/pp/utility.hpp"
#include "ccmath/internal/math/runtime/pp/vec_ext.hpp"
#include "ccmath/internal/predef/attributes/always_inline.hpp"

#include <cstddef>
#include <type_traits>

namespace ccm::pp
{
	// [simd.class] A data-parallel value of N lanes of T. C++26 names ported to
	// C++17; storage and operations come from the per-ABI SimdTraits backend.
	template <typename T, typename Abi>
	class basic_simd
	{
		using Traits	  = SimdTraits<T, Abi>;
		using member_type = typename Traits::SimdMember;

		member_type data_;

	public:
		using value_type = T;
		using abi_type	 = Abi;
		using mask_type	 = basic_simd_mask<sizeof(T), Abi>;

		[[nodiscard]] static constexpr detail::SimdSizeType size() { return Traits::size; }

		basic_simd() = default;

		// Broadcast: a value convertible to T fills every lane (implicit, per spec).
		CCM_ALWAYS_INLINE basic_simd(T value) : data_(Traits::broadcast(value)) {} // NOLINT(google-explicit-constructor)

		// Generator: gen(integral_constant<i>) supplies lane i.
		template <typename G,
				  std::enable_if_t<!std::is_convertible_v<G, T> &&
									   std::is_convertible_v<decltype(std::declval<G &>()(std::integral_constant<detail::SimdSizeType, 0>{})), T>,
								   int> = 0>
		CCM_ALWAYS_INLINE explicit basic_simd(G &&gen) // NOLINT(cppcoreguidelines-missing-std-forward)
		{
			detail::unroll_ic<size()>([&](auto i) { Traits::set(data_, i, static_cast<T>(gen(i))); });
		}

		// Converting constructor: same lane count, different element type, per-lane
		// static_cast. Explicit (we do not model the standard's value-preserving
		// implicit-conversion rules).
		template <typename U, typename A2, std::enable_if_t<(basic_simd<U, A2>::size() == size()) && !std::is_same_v<basic_simd<U, A2>, basic_simd>, int> = 0>
		CCM_ALWAYS_INLINE explicit basic_simd(basic_simd<U, A2> const &o)
		{
			for (detail::SimdSizeType i = 0; i < size(); ++i) { (*this)[i] = static_cast<T>(o[i]); }
		}

		// Load from contiguous memory.
		template <typename Flags = simd_flags<>, std::enable_if_t<is_simd_flag_type_v<Flags>, int> = 0>
		CCM_ALWAYS_INLINE basic_simd(T const *ptr, Flags flags = {}) // NOLINT(google-explicit-constructor)
		{ copy_from(ptr, flags); }

		template <typename Flags = simd_flags<>>
		CCM_ALWAYS_INLINE void copy_from(T const *ptr, Flags /*flags*/ = {})
		{
			constexpr std::size_t req = detail::flags_align_request<Flags>::value;
			if constexpr (req != 0) { ptr = detail::assume_aligned_ptr<(req == 1 ? Traits::simd_align : req)>(ptr); }
			data_ = Traits::load(ptr);
		}
		template <typename Flags = simd_flags<>>
		CCM_ALWAYS_INLINE void copy_to(T *ptr, Flags /*flags*/ = {}) const
		{
			constexpr std::size_t req = detail::flags_align_request<Flags>::value;
			if constexpr (req != 0) { ptr = detail::assume_aligned_ptr<(req == 1 ? Traits::simd_align : req)>(ptr); }
			Traits::store(data_, ptr);
		}

		// Element access. Const returns by value (C++26); the mutable overload
		// returns a lane proxy (TS-style extension).
		CCM_ALWAYS_INLINE value_type operator[](detail::SimdSizeType i) const { return Traits::get(data_, i); }
		CCM_ALWAYS_INLINE detail::SimdReference<T, Abi> operator[](detail::SimdSizeType i) { return { &data_, i }; }

		CCM_ALWAYS_INLINE static basic_simd from_member(member_type const &m)
		{
			basic_simd r;
			r.data_ = m;
			return r;
		}
		[[nodiscard]] CCM_ALWAYS_INLINE member_type const &get() const { return data_; }
		CCM_ALWAYS_INLINE member_type &get() { return data_; }

		CCM_ALWAYS_INLINE friend basic_simd operator+(basic_simd const &a, basic_simd const &b) { return from_member(Traits::add(a.data_, b.data_)); }
		CCM_ALWAYS_INLINE friend basic_simd operator-(basic_simd const &a, basic_simd const &b) { return from_member(Traits::sub(a.data_, b.data_)); }
		CCM_ALWAYS_INLINE friend basic_simd operator*(basic_simd const &a, basic_simd const &b) { return from_member(Traits::mul(a.data_, b.data_)); }
		CCM_ALWAYS_INLINE friend basic_simd operator/(basic_simd const &a, basic_simd const &b) { return from_member(Traits::div(a.data_, b.data_)); }
		CCM_ALWAYS_INLINE basic_simd operator-() const { return from_member(Traits::negate(data_)); }
		CCM_ALWAYS_INLINE basic_simd operator+() const { return *this; }

		CCM_ALWAYS_INLINE basic_simd &operator+=(basic_simd const &o)
		{
			data_ = Traits::add(data_, o.data_);
			return *this;
		}
		CCM_ALWAYS_INLINE basic_simd &operator-=(basic_simd const &o)
		{
			data_ = Traits::sub(data_, o.data_);
			return *this;
		}
		CCM_ALWAYS_INLINE basic_simd &operator*=(basic_simd const &o)
		{
			data_ = Traits::mul(data_, o.data_);
			return *this;
		}
		CCM_ALWAYS_INLINE basic_simd &operator/=(basic_simd const &o)
		{
			data_ = Traits::div(data_, o.data_);
			return *this;
		}

		template <typename U = T, std::enable_if_t<std::is_integral_v<U>, int> = 0>
		CCM_ALWAYS_INLINE friend basic_simd operator%(basic_simd const &a, basic_simd const &b)
		{ return from_member(Traits::mod(a.data_, b.data_)); }
		template <typename U = T, std::enable_if_t<std::is_integral_v<U>, int> = 0>
		CCM_ALWAYS_INLINE friend basic_simd operator&(basic_simd const &a, basic_simd const &b)
		{ return from_member(Traits::band(a.data_, b.data_)); }
		template <typename U = T, std::enable_if_t<std::is_integral_v<U>, int> = 0>
		CCM_ALWAYS_INLINE friend basic_simd operator|(basic_simd const &a, basic_simd const &b)
		{ return from_member(Traits::bor(a.data_, b.data_)); }
		template <typename U = T, std::enable_if_t<std::is_integral_v<U>, int> = 0>
		CCM_ALWAYS_INLINE friend basic_simd operator^(basic_simd const &a, basic_simd const &b)
		{ return from_member(Traits::bxor(a.data_, b.data_)); }
		template <typename U = T, std::enable_if_t<std::is_integral_v<U>, int> = 0>
		CCM_ALWAYS_INLINE friend basic_simd operator<<(basic_simd const &a, basic_simd const &b)
		{ return from_member(Traits::shl(a.data_, b.data_)); }
		template <typename U = T, std::enable_if_t<std::is_integral_v<U>, int> = 0>
		CCM_ALWAYS_INLINE friend basic_simd operator>>(basic_simd const &a, basic_simd const &b)
		{ return from_member(Traits::shr(a.data_, b.data_)); }
		template <typename U = T, std::enable_if_t<std::is_integral_v<U>, int> = 0>
		CCM_ALWAYS_INLINE basic_simd operator~() const
		{ return from_member(Traits::bnot(data_)); }

		template <typename U = T, std::enable_if_t<std::is_integral_v<U>, int> = 0>
		CCM_ALWAYS_INLINE basic_simd &operator%=(basic_simd const &o)
		{
			data_ = Traits::mod(data_, o.data_);
			return *this;
		}
		template <typename U = T, std::enable_if_t<std::is_integral_v<U>, int> = 0>
		CCM_ALWAYS_INLINE basic_simd &operator&=(basic_simd const &o)
		{
			data_ = Traits::band(data_, o.data_);
			return *this;
		}
		template <typename U = T, std::enable_if_t<std::is_integral_v<U>, int> = 0>
		CCM_ALWAYS_INLINE basic_simd &operator|=(basic_simd const &o)
		{
			data_ = Traits::bor(data_, o.data_);
			return *this;
		}
		template <typename U = T, std::enable_if_t<std::is_integral_v<U>, int> = 0>
		CCM_ALWAYS_INLINE basic_simd &operator^=(basic_simd const &o)
		{
			data_ = Traits::bxor(data_, o.data_);
			return *this;
		}
		template <typename U = T, std::enable_if_t<std::is_integral_v<U>, int> = 0>
		CCM_ALWAYS_INLINE basic_simd &operator<<=(basic_simd const &o)
		{
			data_ = Traits::shl(data_, o.data_);
			return *this;
		}
		template <typename U = T, std::enable_if_t<std::is_integral_v<U>, int> = 0>
		CCM_ALWAYS_INLINE basic_simd &operator>>=(basic_simd const &o)
		{
			data_ = Traits::shr(data_, o.data_);
			return *this;
		}

		CCM_ALWAYS_INLINE friend mask_type operator==(basic_simd const &a, basic_simd const &b) { return mask_type::from_member(Traits::eq(a.data_, b.data_)); }
		CCM_ALWAYS_INLINE friend mask_type operator!=(basic_simd const &a, basic_simd const &b) { return mask_type::from_member(Traits::ne(a.data_, b.data_)); }
		CCM_ALWAYS_INLINE friend mask_type operator<(basic_simd const &a, basic_simd const &b) { return mask_type::from_member(Traits::lt(a.data_, b.data_)); }
		CCM_ALWAYS_INLINE friend mask_type operator<=(basic_simd const &a, basic_simd const &b) { return mask_type::from_member(Traits::le(a.data_, b.data_)); }
		CCM_ALWAYS_INLINE friend mask_type operator>(basic_simd const &a, basic_simd const &b) { return mask_type::from_member(Traits::gt(a.data_, b.data_)); }
		CCM_ALWAYS_INLINE friend mask_type operator>=(basic_simd const &a, basic_simd const &b) { return mask_type::from_member(Traits::ge(a.data_, b.data_)); }
	};
} // namespace ccm::pp
