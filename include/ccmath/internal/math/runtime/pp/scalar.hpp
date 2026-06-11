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

#include <cstddef>
#include <cstring>

// Scalar backend: a single-lane storage that mirrors libc++'s scalar.h. Every
// operation degrades to the plain scalar operation on T.

namespace ccm::pp
{
	template <typename T>
	struct SimdTraits<T, ScalarAbi>
	{
		using value_type						   = T;
		using SimdMember						   = T;
		using MaskMember						   = bool;
		static constexpr detail::SimdSizeType size = 1;
		static constexpr std::size_t simd_align	   = alignof(T);
		static constexpr std::size_t mask_align	   = alignof(bool);

		CCM_ALWAYS_INLINE static SimdMember broadcast(T v) { return v; }
		CCM_ALWAYS_INLINE static T get(SimdMember const & m, detail::SimdSizeType /*i*/) { return m; }
		CCM_ALWAYS_INLINE static void set(SimdMember & m, detail::SimdSizeType /*i*/, T v) { m = v; }
		CCM_ALWAYS_INLINE static SimdMember load(T const * p) { return *p; }
		CCM_ALWAYS_INLINE static void store(SimdMember const & m, T * p) { *p = m; }

		CCM_ALWAYS_INLINE static SimdMember add(SimdMember a, SimdMember b) { return a + b; }
		CCM_ALWAYS_INLINE static SimdMember sub(SimdMember a, SimdMember b) { return a - b; }
		CCM_ALWAYS_INLINE static SimdMember mul(SimdMember a, SimdMember b) { return a * b; }
		CCM_ALWAYS_INLINE static SimdMember div(SimdMember a, SimdMember b) { return a / b; }
		CCM_ALWAYS_INLINE static SimdMember negate(SimdMember a) { return -a; }

		CCM_ALWAYS_INLINE static MaskMember eq(SimdMember a, SimdMember b) { return a == b; }
		CCM_ALWAYS_INLINE static MaskMember ne(SimdMember a, SimdMember b) { return a != b; }
		CCM_ALWAYS_INLINE static MaskMember lt(SimdMember a, SimdMember b) { return a < b; }
		CCM_ALWAYS_INLINE static MaskMember le(SimdMember a, SimdMember b) { return a <= b; }
		CCM_ALWAYS_INLINE static MaskMember gt(SimdMember a, SimdMember b) { return a > b; }
		CCM_ALWAYS_INLINE static MaskMember ge(SimdMember a, SimdMember b) { return a >= b; }

		CCM_ALWAYS_INLINE static MaskMember mbroadcast(bool b) { return b; }
		CCM_ALWAYS_INLINE static bool mget(MaskMember const & m, detail::SimdSizeType /*i*/) { return m; }
		CCM_ALWAYS_INLINE static void mset(MaskMember & m, detail::SimdSizeType /*i*/, bool b) { m = b; }
		CCM_ALWAYS_INLINE static MaskMember mand(MaskMember a, MaskMember b) { return a && b; }
		CCM_ALWAYS_INLINE static MaskMember mor(MaskMember a, MaskMember b) { return a || b; }
		CCM_ALWAYS_INLINE static MaskMember mxor(MaskMember a, MaskMember b) { return a != b; }
		CCM_ALWAYS_INLINE static MaskMember mnot(MaskMember a) { return !a; }

		CCM_ALWAYS_INLINE static bool all_of(MaskMember m) { return m; }
		CCM_ALWAYS_INLINE static bool any_of(MaskMember m) { return m; }
		CCM_ALWAYS_INLINE static detail::SimdSizeType popcount(MaskMember m) { return m ? 1 : 0; }

		CCM_ALWAYS_INLINE static SimdMember select(MaskMember m, SimdMember a, SimdMember b) { return m ? a : b; }

		// Integer-only operations (instantiated only when used, i.e. for integral T).
		CCM_ALWAYS_INLINE static SimdMember mod(SimdMember a, SimdMember b) { return a % b; }
		CCM_ALWAYS_INLINE static SimdMember band(SimdMember a, SimdMember b) { return a & b; }
		CCM_ALWAYS_INLINE static SimdMember bor(SimdMember a, SimdMember b) { return a | b; }
		CCM_ALWAYS_INLINE static SimdMember bxor(SimdMember a, SimdMember b) { return a ^ b; }
		CCM_ALWAYS_INLINE static SimdMember bnot(SimdMember a) { return static_cast<SimdMember>(~a); }
		CCM_ALWAYS_INLINE static SimdMember shl(SimdMember a, SimdMember b) { return static_cast<SimdMember>(a << b); }
		CCM_ALWAYS_INLINE static SimdMember shr(SimdMember a, SimdMember b) { return static_cast<SimdMember>(a >> b); }

		// Lane-type conversion and bit reinterpret.
		template <typename U>
		CCM_ALWAYS_INLINE static U convert(SimdMember v)
		{ return static_cast<U>(v); }
		template <typename U>
		CCM_ALWAYS_INLINE static U bitcast(SimdMember v)
		{
			U out;
			std::memcpy(&out, &v, sizeof(U));
			return out;
		}

		// Math primitives.
		CCM_ALWAYS_INLINE static SimdMember op_sqrt(SimdMember v) { return detail::s_sqrt<T>(v); }
		CCM_ALWAYS_INLINE static SimdMember op_floor(SimdMember v) { return detail::s_floor<T>(v); }
		CCM_ALWAYS_INLINE static SimdMember op_ceil(SimdMember v) { return detail::s_ceil<T>(v); }
		CCM_ALWAYS_INLINE static SimdMember op_trunc(SimdMember v) { return detail::s_trunc<T>(v); }
		CCM_ALWAYS_INLINE static SimdMember op_round(SimdMember v) { return detail::s_round<T>(v); }
		CCM_ALWAYS_INLINE static SimdMember op_fabs(SimdMember v) { return detail::s_fabs<T>(v); }
		CCM_ALWAYS_INLINE static SimdMember op_fma(SimdMember a, SimdMember b, SimdMember c) { return detail::s_fma<T>(a, b, c); }
		CCM_ALWAYS_INLINE static SimdMember op_min(SimdMember a, SimdMember b) { return a < b ? a : b; }
		CCM_ALWAYS_INLINE static SimdMember op_max(SimdMember a, SimdMember b) { return a < b ? b : a; }

		// Horizontal reductions (a single lane reduces to itself).
		CCM_ALWAYS_INLINE static T hadd(SimdMember v) { return v; }
		CCM_ALWAYS_INLINE static T hmin(SimdMember v) { return v; }
		CCM_ALWAYS_INLINE static T hmax(SimdMember v) { return v; }
	};
} // namespace ccm::pp
