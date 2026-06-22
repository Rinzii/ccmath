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
#include "ccmath/internal/math/runtime/pp/utility.hpp"
#include "ccmath/internal/math/runtime/pp/vec_ext.hpp"
#include "ccmath/internal/predef/attributes/always_inline.hpp"

#include <type_traits>

// Elementwise math overloads for basic_simd. The hardware-mapped operations
// (sqrt, floor, ceil, trunc, round, fabs, fma, min, max) route through the
// backend op_* primitives (packed instructions on Clang, per-lane on GCC). The
// transcendentals (exp, log, pow) are a per-lane scalar baseline for now: they
// give the correct result but are not yet vectorized (a future pass can route
// them through a vector implementation or SVML).

namespace ccm::pp
{
	namespace detail
	{
		template <typename T, typename Abi, typename F>
		CCM_ALWAYS_INLINE basic_simd<T, Abi> map1(basic_simd<T, Abi> const & v, F f)
		{
			basic_simd<T, Abi> r;
			for (SimdSizeType i = 0; i < v.size(); ++i) { r[i] = f(v[i]); }
			return r;
		}
		template <typename T, typename Abi, typename F>
		CCM_ALWAYS_INLINE basic_simd<T, Abi> map2(basic_simd<T, Abi> const & a, basic_simd<T, Abi> const & b, F f)
		{
			basic_simd<T, Abi> r;
			for (SimdSizeType i = 0; i < a.size(); ++i) { r[i] = f(a[i], b[i]); }
			return r;
		}
	} // namespace detail

#define CCM_PP_MATH1(NAME, OP)                                                                                                                                 \
	template <typename T, typename Abi, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>                                                                \
	CCM_ALWAYS_INLINE basic_simd<T, Abi> NAME(basic_simd<T, Abi> const & v)                                                                                    \
	{ return basic_simd<T, Abi>::from_member(SimdTraits<T, Abi>::OP(v.get())); }
	CCM_PP_MATH1(sqrt, op_sqrt)
	CCM_PP_MATH1(floor, op_floor)
	CCM_PP_MATH1(ceil, op_ceil)
	CCM_PP_MATH1(trunc, op_trunc)
	CCM_PP_MATH1(round, op_round)
	CCM_PP_MATH1(fabs, op_fabs)
#undef CCM_PP_MATH1

	template <typename T, typename Abi, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
	CCM_ALWAYS_INLINE basic_simd<T, Abi> fma(basic_simd<T, Abi> const & a, basic_simd<T, Abi> const & b, basic_simd<T, Abi> const & c)
	{ return basic_simd<T, Abi>::from_member(SimdTraits<T, Abi>::op_fma(a.get(), b.get(), c.get())); }

	template <typename T, typename Abi>
	CCM_ALWAYS_INLINE basic_simd<T, Abi> min(basic_simd<T, Abi> const & a, basic_simd<T, Abi> const & b)
	{ return basic_simd<T, Abi>::from_member(SimdTraits<T, Abi>::op_min(a.get(), b.get())); }
	template <typename T, typename Abi>
	CCM_ALWAYS_INLINE basic_simd<T, Abi> max(basic_simd<T, Abi> const & a, basic_simd<T, Abi> const & b)
	{ return basic_simd<T, Abi>::from_member(SimdTraits<T, Abi>::op_max(a.get(), b.get())); }

	// abs: floating point reuses the fabs primitive. Integral negates negatives.
	template <typename T, typename Abi, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
	CCM_ALWAYS_INLINE basic_simd<T, Abi> abs(basic_simd<T, Abi> const & v)
	{ return basic_simd<T, Abi>::from_member(SimdTraits<T, Abi>::op_fabs(v.get())); }
	template <typename T, typename Abi, std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>, int> = 0>
	CCM_ALWAYS_INLINE basic_simd<T, Abi> abs(basic_simd<T, Abi> const & v)
	{
		return detail::map1(v, [](T x) { return x < T(0) ? static_cast<T>(-x) : x; });
	}

	// Per-lane transcendental baseline.
	template <typename T, typename Abi, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
	CCM_ALWAYS_INLINE basic_simd<T, Abi> exp(basic_simd<T, Abi> const & v)
	{
		return detail::map1(v, [](T x) { return detail::s_exp<T>(x); });
	}
	template <typename T, typename Abi, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
	CCM_ALWAYS_INLINE basic_simd<T, Abi> log(basic_simd<T, Abi> const & v)
	{
		return detail::map1(v, [](T x) { return detail::s_log<T>(x); });
	}
	template <typename T, typename Abi, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
	CCM_ALWAYS_INLINE basic_simd<T, Abi> pow(basic_simd<T, Abi> const & a, basic_simd<T, Abi> const & b)
	{
		return detail::map2(a, b, [](T x, T y) { return detail::s_pow<T>(x, y); });
	}
} // namespace ccm::pp
