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
#include "ccmath/internal/math/runtime/pp/vec_ext.hpp" // NOLINT(misc-header-include-cycle)
#include "ccmath/internal/predef/attributes/always_inline.hpp"

// SSE2 intrinsic backend for compilers without the GNU/Clang vector extensions
// (i.e. MSVC cl.exe). It specializes the 128-bit native widths (float x4,
// double x2, and the int32 x4 / int64 x2 used by their masks). Hot operations
// map to <immintrin.h> intrinsics. The rest spill to a small stack array, run
// the scalar logic, and reload (correct, used only for cold paths). Anything not
// specialized here keeps the portable array fallback from vec_ext.hpp, so the
// scalar fallback is always available. Validated on Clang via
// CCM_PP_FORCE_MSVC_INTRIN (the intrinsics are cross-compiler).

#if CCM_PP_USE_MSVC_INTRIN

	#include <immintrin.h>

	#include <array>
	#include <cstdint>
	#include <cstring>

namespace ccm::pp
{
	template <> struct SimdTraits<float, VecAbi<4>>
	{
		using value_type						   = float;
		using MaskInt							   = std::int32_t;
		using SimdMember						   = __m128;
		using MaskMember						   = __m128i;
		static constexpr detail::SimdSizeType size = 4;
		static constexpr std::size_t simd_align	   = 16;
		static constexpr std::size_t mask_align	   = 16;

		CCM_ALWAYS_INLINE static SimdMember broadcast(float v)
		{
			return _mm_set1_ps(v);
		}
		CCM_ALWAYS_INLINE static float get(SimdMember const & m, detail::SimdSizeType i)
		{
			std::array<float, 4> a;
			_mm_storeu_ps(a.data(), m);
			return a[i];
		}
		CCM_ALWAYS_INLINE static void set(SimdMember & m, detail::SimdSizeType i, float v)
		{
			std::array<float, 4> a;
			_mm_storeu_ps(a.data(), m);
			a[i] = v;
			m	 = _mm_loadu_ps(a.data());
		}
		CCM_ALWAYS_INLINE static SimdMember load(float const * p)
		{
			return _mm_loadu_ps(p);
		}
		CCM_ALWAYS_INLINE static void store(SimdMember const & m, float * p)
		{
			_mm_storeu_ps(p, m);
		}

		CCM_ALWAYS_INLINE static SimdMember add(SimdMember a, SimdMember b)
		{
			return _mm_add_ps(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember sub(SimdMember a, SimdMember b)
		{
			return _mm_sub_ps(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember mul(SimdMember a, SimdMember b)
		{
			return _mm_mul_ps(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember div(SimdMember a, SimdMember b)
		{
			return _mm_div_ps(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember negate(SimdMember a)
		{
			return _mm_xor_ps(a, _mm_set1_ps(-0.0F));
		}

		CCM_ALWAYS_INLINE static MaskMember eq(SimdMember a, SimdMember b)
		{
			return _mm_castps_si128(_mm_cmpeq_ps(a, b));
		}
		CCM_ALWAYS_INLINE static MaskMember ne(SimdMember a, SimdMember b)
		{
			return _mm_castps_si128(_mm_cmpneq_ps(a, b));
		}
		CCM_ALWAYS_INLINE static MaskMember lt(SimdMember a, SimdMember b)
		{
			return _mm_castps_si128(_mm_cmplt_ps(a, b));
		}
		CCM_ALWAYS_INLINE static MaskMember le(SimdMember a, SimdMember b)
		{
			return _mm_castps_si128(_mm_cmple_ps(a, b));
		}
		CCM_ALWAYS_INLINE static MaskMember gt(SimdMember a, SimdMember b)
		{
			return _mm_castps_si128(_mm_cmpgt_ps(a, b));
		}
		CCM_ALWAYS_INLINE static MaskMember ge(SimdMember a, SimdMember b)
		{
			return _mm_castps_si128(_mm_cmpge_ps(a, b));
		}

		CCM_ALWAYS_INLINE static MaskMember mbroadcast(bool b)
		{
			return _mm_set1_epi32(b ? -1 : 0);
		}
		CCM_ALWAYS_INLINE static bool mget(MaskMember const & m, detail::SimdSizeType i)
		{
			std::array<std::int32_t, 4> a;
			_mm_storeu_si128(reinterpret_cast<__m128i *>(a.data()), m);
			return a[i] != 0;
		}
		CCM_ALWAYS_INLINE static void mset(MaskMember & m, detail::SimdSizeType i, bool b)
		{
			std::array<std::int32_t, 4> a;
			_mm_storeu_si128(reinterpret_cast<__m128i *>(a.data()), m);
			a[i] = b ? -1 : 0;
			m	 = _mm_loadu_si128(reinterpret_cast<__m128i const *>(a.data()));
		}
		CCM_ALWAYS_INLINE static MaskMember mand(MaskMember a, MaskMember b)
		{
			return _mm_and_si128(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mor(MaskMember a, MaskMember b)
		{
			return _mm_or_si128(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mxor(MaskMember a, MaskMember b)
		{
			return _mm_xor_si128(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mnot(MaskMember a)
		{
			return _mm_xor_si128(a, _mm_set1_epi32(-1));
		}
		CCM_ALWAYS_INLINE static bool all_of(MaskMember m)
		{
			return _mm_movemask_ps(_mm_castsi128_ps(m)) == 0xF;
		}
		CCM_ALWAYS_INLINE static bool any_of(MaskMember m)
		{
			return _mm_movemask_ps(_mm_castsi128_ps(m)) != 0;
		}
		CCM_ALWAYS_INLINE static detail::SimdSizeType popcount(MaskMember m)
		{
			std::array<std::int32_t, 4> a;
			_mm_storeu_si128(reinterpret_cast<__m128i *>(a.data()), m);
			detail::SimdSizeType c = 0;
			for (int i = 0; i < 4; ++i)
			{
				c += (a[i] != 0) ? 1 : 0;
			}
			return c;
		}
		CCM_ALWAYS_INLINE static SimdMember select(MaskMember m, SimdMember a, SimdMember b)
		{
			__m128 const mps = _mm_castsi128_ps(m);
			return _mm_or_ps(_mm_and_ps(mps, a), _mm_andnot_ps(mps, b));
		}

		CCM_ALWAYS_INLINE static SimdMember op_sqrt(SimdMember v)
		{
			return _mm_sqrt_ps(v);
		}
		CCM_ALWAYS_INLINE static SimdMember op_fabs(SimdMember v)
		{
			return _mm_andnot_ps(_mm_set1_ps(-0.0F), v);
		}
		CCM_ALWAYS_INLINE static SimdMember op_min(SimdMember a, SimdMember b)
		{
			return _mm_min_ps(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember op_max(SimdMember a, SimdMember b)
		{
			return _mm_max_ps(a, b);
		}
	#define CCM_PP_MSVC_F32_ROUND(NAME, SFN)                                                                                                                   \
		CCM_ALWAYS_INLINE static SimdMember NAME(SimdMember v)                                                                                                 \
		{                                                                                                                                                      \
			std::array<float, 4> a;                                                                                                                            \
			_mm_storeu_ps(a.data(), v);                                                                                                                        \
			for (int i = 0; i < 4; ++i)                                                                                                                        \
			{                                                                                                                                                  \
				a[i] = detail::SFN<float>(a[i]);                                                                                                               \
			}                                                                                                                                                  \
			return _mm_loadu_ps(a.data());                                                                                                                     \
		}
		CCM_PP_MSVC_F32_ROUND(op_floor, s_floor)
		CCM_PP_MSVC_F32_ROUND(op_ceil, s_ceil)
		CCM_PP_MSVC_F32_ROUND(op_trunc, s_trunc)
		CCM_PP_MSVC_F32_ROUND(op_round, s_round)
	#undef CCM_PP_MSVC_F32_ROUND
		CCM_ALWAYS_INLINE static SimdMember op_fma(SimdMember a, SimdMember b, SimdMember c)
		{
			std::array<float, 4> aa, bb, cc;
			_mm_storeu_ps(aa.data(), a);
			_mm_storeu_ps(bb.data(), b);
			_mm_storeu_ps(cc.data(), c);
			for (int i = 0; i < 4; ++i)
			{
				aa[i] = detail::s_fma<float>(aa[i], bb[i], cc[i]);
			}
			return _mm_loadu_ps(aa.data());
		}
		CCM_ALWAYS_INLINE static float hadd(SimdMember v)
		{
			std::array<float, 4> a;
			_mm_storeu_ps(a.data(), v);
			return a[0] + a[1] + a[2] + a[3];
		}
		CCM_ALWAYS_INLINE static float hmin(SimdMember v)
		{
			std::array<float, 4> a;
			_mm_storeu_ps(a.data(), v);
			float s = a[0];
			for (int i = 1; i < 4; ++i)
			{
				s = a[i] < s ? a[i] : s;
			}
			return s;
		}
		CCM_ALWAYS_INLINE static float hmax(SimdMember v)
		{
			std::array<float, 4> a;
			_mm_storeu_ps(a.data(), v);
			float s = a[0];
			for (int i = 1; i < 4; ++i)
			{
				s = a[i] > s ? a[i] : s;
			}
			return s;
		}

		template <typename U> CCM_ALWAYS_INLINE static typename SimdTraits<U, VecAbi<4>>::SimdMember convert(SimdMember v)
		{
			std::array<float, 4> a;
			_mm_storeu_ps(a.data(), v);
			typename SimdTraits<U, VecAbi<4>>::SimdMember r;
			for (detail::SimdSizeType i = 0; i < 4; ++i)
			{
				SimdTraits<U, VecAbi<4>>::set(r, i, static_cast<U>(a[i]));
			}
			return r;
		}
		template <typename U> CCM_ALWAYS_INLINE static typename SimdTraits<U, VecAbi<4>>::SimdMember bitcast(SimdMember v)
		{
			typename SimdTraits<U, VecAbi<4>>::SimdMember r;
			std::memcpy(&r, &v, sizeof(r));
			return r;
		}
	};

	template <> struct SimdTraits<double, VecAbi<2>>
	{
		using value_type						   = double;
		using MaskInt							   = std::int64_t;
		using SimdMember						   = __m128d;
		using MaskMember						   = __m128i;
		static constexpr detail::SimdSizeType size = 2;
		static constexpr std::size_t simd_align	   = 16;
		static constexpr std::size_t mask_align	   = 16;

		CCM_ALWAYS_INLINE static SimdMember broadcast(double v)
		{
			return _mm_set1_pd(v);
		}
		CCM_ALWAYS_INLINE static double get(SimdMember const & m, detail::SimdSizeType i)
		{
			std::array<double, 2> a;
			_mm_storeu_pd(a.data(), m);
			return a[i];
		}
		CCM_ALWAYS_INLINE static void set(SimdMember & m, detail::SimdSizeType i, double v)
		{
			std::array<double, 2> a;
			_mm_storeu_pd(a.data(), m);
			a[i] = v;
			m	 = _mm_loadu_pd(a.data());
		}
		CCM_ALWAYS_INLINE static SimdMember load(double const * p)
		{
			return _mm_loadu_pd(p);
		}
		CCM_ALWAYS_INLINE static void store(SimdMember const & m, double * p)
		{
			_mm_storeu_pd(p, m);
		}

		CCM_ALWAYS_INLINE static SimdMember add(SimdMember a, SimdMember b)
		{
			return _mm_add_pd(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember sub(SimdMember a, SimdMember b)
		{
			return _mm_sub_pd(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember mul(SimdMember a, SimdMember b)
		{
			return _mm_mul_pd(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember div(SimdMember a, SimdMember b)
		{
			return _mm_div_pd(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember negate(SimdMember a)
		{
			return _mm_xor_pd(a, _mm_set1_pd(-0.0));
		}

		CCM_ALWAYS_INLINE static MaskMember eq(SimdMember a, SimdMember b)
		{
			return _mm_castpd_si128(_mm_cmpeq_pd(a, b));
		}
		CCM_ALWAYS_INLINE static MaskMember ne(SimdMember a, SimdMember b)
		{
			return _mm_castpd_si128(_mm_cmpneq_pd(a, b));
		}
		CCM_ALWAYS_INLINE static MaskMember lt(SimdMember a, SimdMember b)
		{
			return _mm_castpd_si128(_mm_cmplt_pd(a, b));
		}
		CCM_ALWAYS_INLINE static MaskMember le(SimdMember a, SimdMember b)
		{
			return _mm_castpd_si128(_mm_cmple_pd(a, b));
		}
		CCM_ALWAYS_INLINE static MaskMember gt(SimdMember a, SimdMember b)
		{
			return _mm_castpd_si128(_mm_cmpgt_pd(a, b));
		}
		CCM_ALWAYS_INLINE static MaskMember ge(SimdMember a, SimdMember b)
		{
			return _mm_castpd_si128(_mm_cmpge_pd(a, b));
		}

		CCM_ALWAYS_INLINE static MaskMember mbroadcast(bool b)
		{
			return _mm_set1_epi64x(b ? -1 : 0);
		}
		CCM_ALWAYS_INLINE static bool mget(MaskMember const & m, detail::SimdSizeType i)
		{
			std::array<std::int64_t, 2> a;
			_mm_storeu_si128(reinterpret_cast<__m128i *>(a.data()), m);
			return a[i] != 0;
		}
		CCM_ALWAYS_INLINE static void mset(MaskMember & m, detail::SimdSizeType i, bool b)
		{
			std::array<std::int64_t, 2> a;
			_mm_storeu_si128(reinterpret_cast<__m128i *>(a.data()), m);
			a[i] = b ? -1 : 0;
			m	 = _mm_loadu_si128(reinterpret_cast<__m128i const *>(a.data()));
		}
		CCM_ALWAYS_INLINE static MaskMember mand(MaskMember a, MaskMember b)
		{
			return _mm_and_si128(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mor(MaskMember a, MaskMember b)
		{
			return _mm_or_si128(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mxor(MaskMember a, MaskMember b)
		{
			return _mm_xor_si128(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mnot(MaskMember a)
		{
			return _mm_xor_si128(a, _mm_set1_epi32(-1));
		}
		CCM_ALWAYS_INLINE static bool all_of(MaskMember m)
		{
			return _mm_movemask_pd(_mm_castsi128_pd(m)) == 0x3;
		}
		CCM_ALWAYS_INLINE static bool any_of(MaskMember m)
		{
			return _mm_movemask_pd(_mm_castsi128_pd(m)) != 0;
		}
		CCM_ALWAYS_INLINE static detail::SimdSizeType popcount(MaskMember m)
		{
			std::array<std::int64_t, 2> a;
			_mm_storeu_si128(reinterpret_cast<__m128i *>(a.data()), m);
			return (a[0] != 0 ? 1 : 0) + (a[1] != 0 ? 1 : 0);
		}
		CCM_ALWAYS_INLINE static SimdMember select(MaskMember m, SimdMember a, SimdMember b)
		{
			__m128d const mpd = _mm_castsi128_pd(m);
			return _mm_or_pd(_mm_and_pd(mpd, a), _mm_andnot_pd(mpd, b));
		}

		CCM_ALWAYS_INLINE static SimdMember op_sqrt(SimdMember v)
		{
			return _mm_sqrt_pd(v);
		}
		CCM_ALWAYS_INLINE static SimdMember op_fabs(SimdMember v)
		{
			return _mm_andnot_pd(_mm_set1_pd(-0.0), v);
		}
		CCM_ALWAYS_INLINE static SimdMember op_min(SimdMember a, SimdMember b)
		{
			return _mm_min_pd(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember op_max(SimdMember a, SimdMember b)
		{
			return _mm_max_pd(a, b);
		}
	#define CCM_PP_MSVC_F64_ROUND(NAME, SFN)                                                                                                                   \
		CCM_ALWAYS_INLINE static SimdMember NAME(SimdMember v)                                                                                                 \
		{                                                                                                                                                      \
			std::array<double, 2> a;                                                                                                                           \
			_mm_storeu_pd(a.data(), v);                                                                                                                        \
			a[0] = detail::SFN<double>(a[0]);                                                                                                                  \
			a[1] = detail::SFN<double>(a[1]);                                                                                                                  \
			return _mm_loadu_pd(a.data());                                                                                                                     \
		}
		CCM_PP_MSVC_F64_ROUND(op_floor, s_floor)
		CCM_PP_MSVC_F64_ROUND(op_ceil, s_ceil)
		CCM_PP_MSVC_F64_ROUND(op_trunc, s_trunc)
		CCM_PP_MSVC_F64_ROUND(op_round, s_round)
	#undef CCM_PP_MSVC_F64_ROUND
		CCM_ALWAYS_INLINE static SimdMember op_fma(SimdMember a, SimdMember b, SimdMember c)
		{
			std::array<double, 2> aa, bb, cc;
			_mm_storeu_pd(aa.data(), a);
			_mm_storeu_pd(bb.data(), b);
			_mm_storeu_pd(cc.data(), c);
			aa[0] = detail::s_fma<double>(aa[0], bb[0], cc[0]);
			aa[1] = detail::s_fma<double>(aa[1], bb[1], cc[1]);
			return _mm_loadu_pd(aa.data());
		}
		CCM_ALWAYS_INLINE static double hadd(SimdMember v)
		{
			std::array<double, 2> a;
			_mm_storeu_pd(a.data(), v);
			return a[0] + a[1];
		}
		CCM_ALWAYS_INLINE static double hmin(SimdMember v)
		{
			std::array<double, 2> a;
			_mm_storeu_pd(a.data(), v);
			return a[0] < a[1] ? a[0] : a[1];
		}
		CCM_ALWAYS_INLINE static double hmax(SimdMember v)
		{
			std::array<double, 2> a;
			_mm_storeu_pd(a.data(), v);
			return a[0] > a[1] ? a[0] : a[1];
		}

		template <typename U> CCM_ALWAYS_INLINE static typename SimdTraits<U, VecAbi<2>>::SimdMember convert(SimdMember v)
		{
			std::array<double, 2> a;
			_mm_storeu_pd(a.data(), v);
			typename SimdTraits<U, VecAbi<2>>::SimdMember r;
			for (detail::SimdSizeType i = 0; i < 2; ++i)
			{
				SimdTraits<U, VecAbi<2>>::set(r, i, static_cast<U>(a[i]));
			}
			return r;
		}
		template <typename U> CCM_ALWAYS_INLINE static typename SimdTraits<U, VecAbi<2>>::SimdMember bitcast(SimdMember v)
		{
			typename SimdTraits<U, VecAbi<2>>::SimdMember r;
			std::memcpy(&r, &v, sizeof(r));
			return r;
		}
	};

	namespace detail
	{
		// Shared spill helpers for the integer __m128i specializations.
		template <typename T, int N> CCM_ALWAYS_INLINE void si128_store(__m128i v, T * a)
		{
			_mm_storeu_si128(reinterpret_cast<__m128i *>(a), v);
		}
		template <typename T> CCM_ALWAYS_INLINE __m128i si128_load(T const * a)
		{
			return _mm_loadu_si128(reinterpret_cast<__m128i const *>(a));
		}
	} // namespace detail

	template <> struct SimdTraits<std::int32_t, VecAbi<4>>
	{
		using value_type						   = std::int32_t;
		using MaskInt							   = std::int32_t;
		using SimdMember						   = __m128i;
		using MaskMember						   = __m128i;
		static constexpr detail::SimdSizeType size = 4;
		static constexpr std::size_t simd_align	   = 16;
		static constexpr std::size_t mask_align	   = 16;

		CCM_ALWAYS_INLINE static SimdMember broadcast(std::int32_t v)
		{
			return _mm_set1_epi32(v);
		}
		CCM_ALWAYS_INLINE static std::int32_t get(SimdMember const & m, detail::SimdSizeType i)
		{
			std::array<std::int32_t, 4> a;
			detail::si128_store<std::int32_t, 4>(m, a.data());
			return a[i];
		}
		CCM_ALWAYS_INLINE static void set(SimdMember & m, detail::SimdSizeType i, std::int32_t v)
		{
			std::array<std::int32_t, 4> a;
			detail::si128_store<std::int32_t, 4>(m, a.data());
			a[i] = v;
			m	 = detail::si128_load(a.data());
		}
		CCM_ALWAYS_INLINE static SimdMember load(std::int32_t const * p)
		{
			return detail::si128_load(p);
		}
		CCM_ALWAYS_INLINE static void store(SimdMember const & m, std::int32_t * p)
		{
			detail::si128_store<std::int32_t, 4>(m, p);
		}

		CCM_ALWAYS_INLINE static SimdMember add(SimdMember a, SimdMember b)
		{
			return _mm_add_epi32(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember sub(SimdMember a, SimdMember b)
		{
			return _mm_sub_epi32(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember negate(SimdMember a)
		{
			return _mm_sub_epi32(_mm_setzero_si128(), a);
		}

		CCM_ALWAYS_INLINE static MaskMember eq(SimdMember a, SimdMember b)
		{
			return _mm_cmpeq_epi32(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember lt(SimdMember a, SimdMember b)
		{
			return _mm_cmplt_epi32(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember gt(SimdMember a, SimdMember b)
		{
			return _mm_cmpgt_epi32(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember ne(SimdMember a, SimdMember b)
		{
			return mnot(eq(a, b));
		}
		CCM_ALWAYS_INLINE static MaskMember le(SimdMember a, SimdMember b)
		{
			return mnot(gt(a, b));
		}
		CCM_ALWAYS_INLINE static MaskMember ge(SimdMember a, SimdMember b)
		{
			return mnot(lt(a, b));
		}

		CCM_ALWAYS_INLINE static MaskMember mbroadcast(bool b)
		{
			return _mm_set1_epi32(b ? -1 : 0);
		}
		CCM_ALWAYS_INLINE static bool mget(MaskMember const & m, detail::SimdSizeType i)
		{
			return get(m, i) != 0;
		}
		CCM_ALWAYS_INLINE static void mset(MaskMember & m, detail::SimdSizeType i, bool b)
		{
			set(m, i, b ? -1 : 0);
		}
		CCM_ALWAYS_INLINE static MaskMember mand(MaskMember a, MaskMember b)
		{
			return _mm_and_si128(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mor(MaskMember a, MaskMember b)
		{
			return _mm_or_si128(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mxor(MaskMember a, MaskMember b)
		{
			return _mm_xor_si128(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mnot(MaskMember a)
		{
			return _mm_xor_si128(a, _mm_set1_epi32(-1));
		}
		CCM_ALWAYS_INLINE static bool all_of(MaskMember m)
		{
			return _mm_movemask_ps(_mm_castsi128_ps(m)) == 0xF;
		}
		CCM_ALWAYS_INLINE static bool any_of(MaskMember m)
		{
			return _mm_movemask_ps(_mm_castsi128_ps(m)) != 0;
		}
		CCM_ALWAYS_INLINE static detail::SimdSizeType popcount(MaskMember m)
		{
			std::array<std::int32_t, 4> a;
			detail::si128_store<std::int32_t, 4>(m, a.data());
			detail::SimdSizeType c = 0;
			for (int i = 0; i < 4; ++i)
			{
				c += (a[i] != 0) ? 1 : 0;
			}
			return c;
		}
		CCM_ALWAYS_INLINE static SimdMember select(MaskMember m, SimdMember a, SimdMember b)
		{
			return _mm_or_si128(_mm_and_si128(m, a), _mm_andnot_si128(m, b));
		}

	// Cold integer ops: spill, scalarize, reload.
	#define CCM_PP_MSVC_I32_BIN(NAME, OP)                                                                                                                      \
		CCM_ALWAYS_INLINE static SimdMember NAME(SimdMember a, SimdMember b)                                                                                   \
		{                                                                                                                                                      \
			std::array<std::int32_t, 4> x, y;                                                                                                                  \
			detail::si128_store<std::int32_t, 4>(a, x.data());                                                                                                 \
			detail::si128_store<std::int32_t, 4>(b, y.data());                                                                                                 \
			for (int i = 0; i < 4; ++i)                                                                                                                        \
			{                                                                                                                                                  \
				x[i] = static_cast<std::int32_t>(x[i] OP y[i]);                                                                                                \
			}                                                                                                                                                  \
			return detail::si128_load(x.data());                                                                                                               \
		}
		CCM_PP_MSVC_I32_BIN(mul, *)
		CCM_PP_MSVC_I32_BIN(div, /)
		CCM_PP_MSVC_I32_BIN(mod, %)
		CCM_PP_MSVC_I32_BIN(shl, <<)
		CCM_PP_MSVC_I32_BIN(shr, >>)
	#undef CCM_PP_MSVC_I32_BIN
		CCM_ALWAYS_INLINE static SimdMember band(SimdMember a, SimdMember b)
		{
			return _mm_and_si128(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember bor(SimdMember a, SimdMember b)
		{
			return _mm_or_si128(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember bxor(SimdMember a, SimdMember b)
		{
			return _mm_xor_si128(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember bnot(SimdMember a)
		{
			return _mm_xor_si128(a, _mm_set1_epi32(-1));
		}
		CCM_ALWAYS_INLINE static SimdMember op_min(SimdMember a, SimdMember b)
		{
			return select(lt(a, b), a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember op_max(SimdMember a, SimdMember b)
		{
			return select(gt(a, b), a, b);
		}
		CCM_ALWAYS_INLINE static std::int32_t hadd(SimdMember v)
		{
			std::array<std::int32_t, 4> a;
			detail::si128_store<std::int32_t, 4>(v, a.data());
			return static_cast<std::int32_t>(a[0] + a[1] + a[2] + a[3]);
		}
		CCM_ALWAYS_INLINE static std::int32_t hmin(SimdMember v)
		{
			std::array<std::int32_t, 4> a;
			detail::si128_store<std::int32_t, 4>(v, a.data());
			std::int32_t s = a[0];
			for (int i = 1; i < 4; ++i)
			{
				s = a[i] < s ? a[i] : s;
			}
			return s;
		}
		CCM_ALWAYS_INLINE static std::int32_t hmax(SimdMember v)
		{
			std::array<std::int32_t, 4> a;
			detail::si128_store<std::int32_t, 4>(v, a.data());
			std::int32_t s = a[0];
			for (int i = 1; i < 4; ++i)
			{
				s = a[i] > s ? a[i] : s;
			}
			return s;
		}

		template <typename U> CCM_ALWAYS_INLINE static typename SimdTraits<U, VecAbi<4>>::SimdMember convert(SimdMember v)
		{
			std::array<std::int32_t, 4> a;
			detail::si128_store<std::int32_t, 4>(v, a.data());
			typename SimdTraits<U, VecAbi<4>>::SimdMember r;
			for (detail::SimdSizeType i = 0; i < 4; ++i)
			{
				SimdTraits<U, VecAbi<4>>::set(r, i, static_cast<U>(a[i]));
			}
			return r;
		}
		template <typename U> CCM_ALWAYS_INLINE static typename SimdTraits<U, VecAbi<4>>::SimdMember bitcast(SimdMember v)
		{
			typename SimdTraits<U, VecAbi<4>>::SimdMember r;
			std::memcpy(&r, &v, sizeof(r));
			return r;
		}
	};

	// Mainly the mask type for double. SSE2 has few 64-bit integer ops, so most
	// arithmetic/compares spill.
	template <> struct SimdTraits<std::int64_t, VecAbi<2>>
	{
		using value_type						   = std::int64_t;
		using MaskInt							   = std::int64_t;
		using SimdMember						   = __m128i;
		using MaskMember						   = __m128i;
		static constexpr detail::SimdSizeType size = 2;
		static constexpr std::size_t simd_align	   = 16;
		static constexpr std::size_t mask_align	   = 16;

		CCM_ALWAYS_INLINE static SimdMember broadcast(std::int64_t v)
		{
			return _mm_set1_epi64x(v);
		}
		CCM_ALWAYS_INLINE static std::int64_t get(SimdMember const & m, detail::SimdSizeType i)
		{
			std::array<std::int64_t, 2> a;
			detail::si128_store<std::int64_t, 2>(m, a.data());
			return a[i];
		}
		CCM_ALWAYS_INLINE static void set(SimdMember & m, detail::SimdSizeType i, std::int64_t v)
		{
			std::array<std::int64_t, 2> a;
			detail::si128_store<std::int64_t, 2>(m, a.data());
			a[i] = v;
			m	 = detail::si128_load(a.data());
		}
		CCM_ALWAYS_INLINE static SimdMember load(std::int64_t const * p)
		{
			return detail::si128_load(p);
		}
		CCM_ALWAYS_INLINE static void store(SimdMember const & m, std::int64_t * p)
		{
			detail::si128_store<std::int64_t, 2>(m, p);
		}

		CCM_ALWAYS_INLINE static SimdMember add(SimdMember a, SimdMember b)
		{
			return _mm_add_epi64(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember sub(SimdMember a, SimdMember b)
		{
			return _mm_sub_epi64(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember negate(SimdMember a)
		{
			return _mm_sub_epi64(_mm_setzero_si128(), a);
		}

	#define CCM_PP_MSVC_I64_CMP(NAME, OP)                                                                                                                      \
		CCM_ALWAYS_INLINE static MaskMember NAME(SimdMember a, SimdMember b)                                                                                   \
		{                                                                                                                                                      \
			std::array<std::int64_t, 2> x, y, r;                                                                                                               \
			detail::si128_store<std::int64_t, 2>(a, x.data());                                                                                                 \
			detail::si128_store<std::int64_t, 2>(b, y.data());                                                                                                 \
			r[0] = (x[0] OP y[0]) ? -1 : 0;                                                                                                                    \
			r[1] = (x[1] OP y[1]) ? -1 : 0;                                                                                                                    \
			return detail::si128_load(r.data());                                                                                                               \
		}
		CCM_PP_MSVC_I64_CMP(eq, ==)
		CCM_PP_MSVC_I64_CMP(ne, !=)
		CCM_PP_MSVC_I64_CMP(lt, <)
		CCM_PP_MSVC_I64_CMP(le, <=)
		CCM_PP_MSVC_I64_CMP(gt, >)
		CCM_PP_MSVC_I64_CMP(ge, >=)
	#undef CCM_PP_MSVC_I64_CMP

		CCM_ALWAYS_INLINE static MaskMember mbroadcast(bool b)
		{
			return _mm_set1_epi64x(b ? -1 : 0);
		}
		CCM_ALWAYS_INLINE static bool mget(MaskMember const & m, detail::SimdSizeType i)
		{
			return get(m, i) != 0;
		}
		CCM_ALWAYS_INLINE static void mset(MaskMember & m, detail::SimdSizeType i, bool b)
		{
			set(m, i, b ? -1 : 0);
		}
		CCM_ALWAYS_INLINE static MaskMember mand(MaskMember a, MaskMember b)
		{
			return _mm_and_si128(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mor(MaskMember a, MaskMember b)
		{
			return _mm_or_si128(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mxor(MaskMember a, MaskMember b)
		{
			return _mm_xor_si128(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mnot(MaskMember a)
		{
			return _mm_xor_si128(a, _mm_set1_epi32(-1));
		}
		CCM_ALWAYS_INLINE static bool all_of(MaskMember m)
		{
			return _mm_movemask_pd(_mm_castsi128_pd(m)) == 0x3;
		}
		CCM_ALWAYS_INLINE static bool any_of(MaskMember m)
		{
			return _mm_movemask_pd(_mm_castsi128_pd(m)) != 0;
		}
		CCM_ALWAYS_INLINE static detail::SimdSizeType popcount(MaskMember m)
		{
			std::array<std::int64_t, 2> a;
			detail::si128_store<std::int64_t, 2>(m, a.data());
			return (a[0] != 0 ? 1 : 0) + (a[1] != 0 ? 1 : 0);
		}
		CCM_ALWAYS_INLINE static SimdMember select(MaskMember m, SimdMember a, SimdMember b)
		{
			return _mm_or_si128(_mm_and_si128(m, a), _mm_andnot_si128(m, b));
		}

		CCM_ALWAYS_INLINE static SimdMember band(SimdMember a, SimdMember b)
		{
			return _mm_and_si128(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember bor(SimdMember a, SimdMember b)
		{
			return _mm_or_si128(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember bxor(SimdMember a, SimdMember b)
		{
			return _mm_xor_si128(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember bnot(SimdMember a)
		{
			return _mm_xor_si128(a, _mm_set1_epi32(-1));
		}
	#define CCM_PP_MSVC_I64_BIN(NAME, OP)                                                                                                                      \
		CCM_ALWAYS_INLINE static SimdMember NAME(SimdMember a, SimdMember b)                                                                                   \
		{                                                                                                                                                      \
			std::array<std::int64_t, 2> x, y;                                                                                                                  \
			detail::si128_store<std::int64_t, 2>(a, x.data());                                                                                                 \
			detail::si128_store<std::int64_t, 2>(b, y.data());                                                                                                 \
			x[0] = static_cast<std::int64_t>(x[0] OP y[0]);                                                                                                    \
			x[1] = static_cast<std::int64_t>(x[1] OP y[1]);                                                                                                    \
			return detail::si128_load(x.data());                                                                                                               \
		}
		CCM_PP_MSVC_I64_BIN(mul, *)
		CCM_PP_MSVC_I64_BIN(div, /)
		CCM_PP_MSVC_I64_BIN(mod, %)
		CCM_PP_MSVC_I64_BIN(shl, <<)
		CCM_PP_MSVC_I64_BIN(shr, >>)
	#undef CCM_PP_MSVC_I64_BIN
		CCM_ALWAYS_INLINE static SimdMember op_min(SimdMember a, SimdMember b)
		{
			return select(lt(a, b), a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember op_max(SimdMember a, SimdMember b)
		{
			return select(gt(a, b), a, b);
		}
		CCM_ALWAYS_INLINE static std::int64_t hadd(SimdMember v)
		{
			std::array<std::int64_t, 2> a;
			detail::si128_store<std::int64_t, 2>(v, a.data());
			return static_cast<std::int64_t>(a[0] + a[1]);
		}
		CCM_ALWAYS_INLINE static std::int64_t hmin(SimdMember v)
		{
			std::array<std::int64_t, 2> a;
			detail::si128_store<std::int64_t, 2>(v, a.data());
			return a[0] < a[1] ? a[0] : a[1];
		}
		CCM_ALWAYS_INLINE static std::int64_t hmax(SimdMember v)
		{
			std::array<std::int64_t, 2> a;
			detail::si128_store<std::int64_t, 2>(v, a.data());
			return a[0] > a[1] ? a[0] : a[1];
		}

		template <typename U> CCM_ALWAYS_INLINE static typename SimdTraits<U, VecAbi<2>>::SimdMember convert(SimdMember v)
		{
			std::array<std::int64_t, 2> a;
			detail::si128_store<std::int64_t, 2>(v, a.data());
			typename SimdTraits<U, VecAbi<2>>::SimdMember r;
			for (detail::SimdSizeType i = 0; i < 2; ++i)
			{
				SimdTraits<U, VecAbi<2>>::set(r, i, static_cast<U>(a[i]));
			}
			return r;
		}
		template <typename U> CCM_ALWAYS_INLINE static typename SimdTraits<U, VecAbi<2>>::SimdMember bitcast(SimdMember v)
		{
			typename SimdTraits<U, VecAbi<2>>::SimdMember r;
			std::memcpy(&r, &v, sizeof(r));
			return r;
		}
	};

	// 256-bit native widths. Floating-point ops need only AVX. Integer mask and
	// bitwise ops are done in the float domain (AVX-safe). 256-bit integer
	// arithmetic / compares use AVX2 intrinsics when available and spill otherwise.
	#if CCMATH_SIMD_HAVE_AVX
	namespace detail
	{
		CCM_ALWAYS_INLINE __m256i avx_and(__m256i a, __m256i b)
		{
			return _mm256_castps_si256(_mm256_and_ps(_mm256_castsi256_ps(a), _mm256_castsi256_ps(b)));
		}
		CCM_ALWAYS_INLINE __m256i avx_or(__m256i a, __m256i b)
		{
			return _mm256_castps_si256(_mm256_or_ps(_mm256_castsi256_ps(a), _mm256_castsi256_ps(b)));
		}
		CCM_ALWAYS_INLINE __m256i avx_xor(__m256i a, __m256i b)
		{
			return _mm256_castps_si256(_mm256_xor_ps(_mm256_castsi256_ps(a), _mm256_castsi256_ps(b)));
		}
		CCM_ALWAYS_INLINE __m256i avx_not(__m256i a)
		{
			return avx_xor(a, _mm256_set1_epi32(-1));
		}
		template <typename T> CCM_ALWAYS_INLINE void si256_store(__m256i v, T * a)
		{
			_mm256_storeu_si256(reinterpret_cast<__m256i *>(a), v);
		}
		template <typename T> CCM_ALWAYS_INLINE __m256i si256_load(T const * a)
		{
			return _mm256_loadu_si256(reinterpret_cast<__m256i const *>(a));
		}
	} // namespace detail

	template <> struct SimdTraits<float, VecAbi<8>>
	{
		using value_type						   = float;
		using MaskInt							   = std::int32_t;
		using SimdMember						   = __m256;
		using MaskMember						   = __m256i;
		static constexpr detail::SimdSizeType size = 8;
		static constexpr std::size_t simd_align	   = 32;
		static constexpr std::size_t mask_align	   = 32;

		CCM_ALWAYS_INLINE static SimdMember broadcast(float v)
		{
			return _mm256_set1_ps(v);
		}
		CCM_ALWAYS_INLINE static float get(SimdMember const & m, detail::SimdSizeType i)
		{
			std::array<float, 8> a;
			_mm256_storeu_ps(a.data(), m);
			return a[i];
		}
		CCM_ALWAYS_INLINE static void set(SimdMember & m, detail::SimdSizeType i, float v)
		{
			std::array<float, 8> a;
			_mm256_storeu_ps(a.data(), m);
			a[i] = v;
			m	 = _mm256_loadu_ps(a.data());
		}
		CCM_ALWAYS_INLINE static SimdMember load(float const * p)
		{
			return _mm256_loadu_ps(p);
		}
		CCM_ALWAYS_INLINE static void store(SimdMember const & m, float * p)
		{
			_mm256_storeu_ps(p, m);
		}

		CCM_ALWAYS_INLINE static SimdMember add(SimdMember a, SimdMember b)
		{
			return _mm256_add_ps(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember sub(SimdMember a, SimdMember b)
		{
			return _mm256_sub_ps(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember mul(SimdMember a, SimdMember b)
		{
			return _mm256_mul_ps(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember div(SimdMember a, SimdMember b)
		{
			return _mm256_div_ps(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember negate(SimdMember a)
		{
			return _mm256_xor_ps(a, _mm256_set1_ps(-0.0F));
		}

		CCM_ALWAYS_INLINE static MaskMember eq(SimdMember a, SimdMember b)
		{
			return _mm256_castps_si256(_mm256_cmp_ps(a, b, _CMP_EQ_OQ));
		}
		CCM_ALWAYS_INLINE static MaskMember ne(SimdMember a, SimdMember b)
		{
			return _mm256_castps_si256(_mm256_cmp_ps(a, b, _CMP_NEQ_UQ));
		}
		CCM_ALWAYS_INLINE static MaskMember lt(SimdMember a, SimdMember b)
		{
			return _mm256_castps_si256(_mm256_cmp_ps(a, b, _CMP_LT_OQ));
		}
		CCM_ALWAYS_INLINE static MaskMember le(SimdMember a, SimdMember b)
		{
			return _mm256_castps_si256(_mm256_cmp_ps(a, b, _CMP_LE_OQ));
		}
		CCM_ALWAYS_INLINE static MaskMember gt(SimdMember a, SimdMember b)
		{
			return _mm256_castps_si256(_mm256_cmp_ps(a, b, _CMP_GT_OQ));
		}
		CCM_ALWAYS_INLINE static MaskMember ge(SimdMember a, SimdMember b)
		{
			return _mm256_castps_si256(_mm256_cmp_ps(a, b, _CMP_GE_OQ));
		}

		CCM_ALWAYS_INLINE static MaskMember mbroadcast(bool b)
		{
			return _mm256_set1_epi32(b ? -1 : 0);
		}
		CCM_ALWAYS_INLINE static bool mget(MaskMember const & m, detail::SimdSizeType i)
		{
			std::array<std::int32_t, 8> a;
			detail::si256_store(m, a.data());
			return a[i] != 0;
		}
		CCM_ALWAYS_INLINE static void mset(MaskMember & m, detail::SimdSizeType i, bool b)
		{
			std::array<std::int32_t, 8> a;
			detail::si256_store(m, a.data());
			a[i] = b ? -1 : 0;
			m	 = detail::si256_load(a.data());
		}
		CCM_ALWAYS_INLINE static MaskMember mand(MaskMember a, MaskMember b)
		{
			return detail::avx_and(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mor(MaskMember a, MaskMember b)
		{
			return detail::avx_or(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mxor(MaskMember a, MaskMember b)
		{
			return detail::avx_xor(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mnot(MaskMember a)
		{
			return detail::avx_not(a);
		}
		CCM_ALWAYS_INLINE static bool all_of(MaskMember m)
		{
			return _mm256_movemask_ps(_mm256_castsi256_ps(m)) == 0xFF;
		}
		CCM_ALWAYS_INLINE static bool any_of(MaskMember m)
		{
			return _mm256_movemask_ps(_mm256_castsi256_ps(m)) != 0;
		}
		CCM_ALWAYS_INLINE static detail::SimdSizeType popcount(MaskMember m)
		{
			std::array<std::int32_t, 8> a;
			detail::si256_store(m, a.data());
			detail::SimdSizeType c = 0;
			for (int i = 0; i < 8; ++i)
			{
				c += (a[i] != 0) ? 1 : 0;
			}
			return c;
		}
		CCM_ALWAYS_INLINE static SimdMember select(MaskMember m, SimdMember a, SimdMember b)
		{
			return _mm256_blendv_ps(b, a, _mm256_castsi256_ps(m));
		}

		CCM_ALWAYS_INLINE static SimdMember op_sqrt(SimdMember v)
		{
			return _mm256_sqrt_ps(v);
		}
		CCM_ALWAYS_INLINE static SimdMember op_fabs(SimdMember v)
		{
			return _mm256_andnot_ps(_mm256_set1_ps(-0.0F), v);
		}
		CCM_ALWAYS_INLINE static SimdMember op_min(SimdMember a, SimdMember b)
		{
			return _mm256_min_ps(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember op_max(SimdMember a, SimdMember b)
		{
			return _mm256_max_ps(a, b);
		}
		#define CCM_PP_MSVC_F32X8_ROUND(NAME, SFN)                                                                                                             \
			CCM_ALWAYS_INLINE static SimdMember NAME(SimdMember v)                                                                                             \
			{                                                                                                                                                  \
				std::array<float, 8> a;                                                                                                                        \
				_mm256_storeu_ps(a.data(), v);                                                                                                                 \
				for (int i = 0; i < 8; ++i)                                                                                                                    \
				{                                                                                                                                              \
					a[i] = detail::SFN<float>(a[i]);                                                                                                           \
				}                                                                                                                                              \
				return _mm256_loadu_ps(a.data());                                                                                                              \
			}
		CCM_PP_MSVC_F32X8_ROUND(op_floor, s_floor)
		CCM_PP_MSVC_F32X8_ROUND(op_ceil, s_ceil)
		CCM_PP_MSVC_F32X8_ROUND(op_trunc, s_trunc)
		CCM_PP_MSVC_F32X8_ROUND(op_round, s_round)
		#undef CCM_PP_MSVC_F32X8_ROUND
		CCM_ALWAYS_INLINE static SimdMember op_fma(SimdMember a, SimdMember b, SimdMember c)
		{
		#if CCMATH_SIMD_HAVE_FMA
			return _mm256_fmadd_ps(a, b, c);
		#else
			std::array<float, 8> aa, bb, cc;
			_mm256_storeu_ps(aa.data(), a);
			_mm256_storeu_ps(bb.data(), b);
			_mm256_storeu_ps(cc.data(), c);
			for (int i = 0; i < 8; ++i)
			{
				aa[i] = detail::s_fma<float>(aa[i], bb[i], cc[i]);
			}
			return _mm256_loadu_ps(aa.data());
		#endif
		}
		CCM_ALWAYS_INLINE static float hadd(SimdMember v)
		{
			std::array<float, 8> a;
			_mm256_storeu_ps(a.data(), v);
			float s = 0;
			for (int i = 0; i < 8; ++i)
			{
				s += a[i];
			}
			return s;
		}
		CCM_ALWAYS_INLINE static float hmin(SimdMember v)
		{
			std::array<float, 8> a;
			_mm256_storeu_ps(a.data(), v);
			float s = a[0];
			for (int i = 1; i < 8; ++i)
			{
				s = a[i] < s ? a[i] : s;
			}
			return s;
		}
		CCM_ALWAYS_INLINE static float hmax(SimdMember v)
		{
			std::array<float, 8> a;
			_mm256_storeu_ps(a.data(), v);
			float s = a[0];
			for (int i = 1; i < 8; ++i)
			{
				s = a[i] > s ? a[i] : s;
			}
			return s;
		}
		template <typename U> CCM_ALWAYS_INLINE static typename SimdTraits<U, VecAbi<8>>::SimdMember convert(SimdMember v)
		{
			std::array<float, 8> a;
			_mm256_storeu_ps(a.data(), v);
			typename SimdTraits<U, VecAbi<8>>::SimdMember r;
			for (detail::SimdSizeType i = 0; i < 8; ++i)
			{
				SimdTraits<U, VecAbi<8>>::set(r, i, static_cast<U>(a[i]));
			}
			return r;
		}
		template <typename U> CCM_ALWAYS_INLINE static typename SimdTraits<U, VecAbi<8>>::SimdMember bitcast(SimdMember v)
		{
			typename SimdTraits<U, VecAbi<8>>::SimdMember r;
			std::memcpy(&r, &v, sizeof(r));
			return r;
		}
	};

	template <> struct SimdTraits<double, VecAbi<4>>
	{
		using value_type						   = double;
		using MaskInt							   = std::int64_t;
		using SimdMember						   = __m256d;
		using MaskMember						   = __m256i;
		static constexpr detail::SimdSizeType size = 4;
		static constexpr std::size_t simd_align	   = 32;
		static constexpr std::size_t mask_align	   = 32;

		CCM_ALWAYS_INLINE static SimdMember broadcast(double v)
		{
			return _mm256_set1_pd(v);
		}
		CCM_ALWAYS_INLINE static double get(SimdMember const & m, detail::SimdSizeType i)
		{
			std::array<double, 4> a;
			_mm256_storeu_pd(a.data(), m);
			return a[i];
		}
		CCM_ALWAYS_INLINE static void set(SimdMember & m, detail::SimdSizeType i, double v)
		{
			std::array<double, 4> a;
			_mm256_storeu_pd(a.data(), m);
			a[i] = v;
			m	 = _mm256_loadu_pd(a.data());
		}
		CCM_ALWAYS_INLINE static SimdMember load(double const * p)
		{
			return _mm256_loadu_pd(p);
		}
		CCM_ALWAYS_INLINE static void store(SimdMember const & m, double * p)
		{
			_mm256_storeu_pd(p, m);
		}

		CCM_ALWAYS_INLINE static SimdMember add(SimdMember a, SimdMember b)
		{
			return _mm256_add_pd(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember sub(SimdMember a, SimdMember b)
		{
			return _mm256_sub_pd(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember mul(SimdMember a, SimdMember b)
		{
			return _mm256_mul_pd(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember div(SimdMember a, SimdMember b)
		{
			return _mm256_div_pd(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember negate(SimdMember a)
		{
			return _mm256_xor_pd(a, _mm256_set1_pd(-0.0));
		}

		CCM_ALWAYS_INLINE static MaskMember eq(SimdMember a, SimdMember b)
		{
			return _mm256_castpd_si256(_mm256_cmp_pd(a, b, _CMP_EQ_OQ));
		}
		CCM_ALWAYS_INLINE static MaskMember ne(SimdMember a, SimdMember b)
		{
			return _mm256_castpd_si256(_mm256_cmp_pd(a, b, _CMP_NEQ_UQ));
		}
		CCM_ALWAYS_INLINE static MaskMember lt(SimdMember a, SimdMember b)
		{
			return _mm256_castpd_si256(_mm256_cmp_pd(a, b, _CMP_LT_OQ));
		}
		CCM_ALWAYS_INLINE static MaskMember le(SimdMember a, SimdMember b)
		{
			return _mm256_castpd_si256(_mm256_cmp_pd(a, b, _CMP_LE_OQ));
		}
		CCM_ALWAYS_INLINE static MaskMember gt(SimdMember a, SimdMember b)
		{
			return _mm256_castpd_si256(_mm256_cmp_pd(a, b, _CMP_GT_OQ));
		}
		CCM_ALWAYS_INLINE static MaskMember ge(SimdMember a, SimdMember b)
		{
			return _mm256_castpd_si256(_mm256_cmp_pd(a, b, _CMP_GE_OQ));
		}

		CCM_ALWAYS_INLINE static MaskMember mbroadcast(bool b)
		{
			return _mm256_set1_epi64x(b ? -1 : 0);
		}
		CCM_ALWAYS_INLINE static bool mget(MaskMember const & m, detail::SimdSizeType i)
		{
			std::array<std::int64_t, 4> a;
			detail::si256_store(m, a.data());
			return a[i] != 0;
		}
		CCM_ALWAYS_INLINE static void mset(MaskMember & m, detail::SimdSizeType i, bool b)
		{
			std::array<std::int64_t, 4> a;
			detail::si256_store(m, a.data());
			a[i] = b ? -1 : 0;
			m	 = detail::si256_load(a.data());
		}
		CCM_ALWAYS_INLINE static MaskMember mand(MaskMember a, MaskMember b)
		{
			return detail::avx_and(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mor(MaskMember a, MaskMember b)
		{
			return detail::avx_or(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mxor(MaskMember a, MaskMember b)
		{
			return detail::avx_xor(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mnot(MaskMember a)
		{
			return detail::avx_not(a);
		}
		CCM_ALWAYS_INLINE static bool all_of(MaskMember m)
		{
			return _mm256_movemask_pd(_mm256_castsi256_pd(m)) == 0xF;
		}
		CCM_ALWAYS_INLINE static bool any_of(MaskMember m)
		{
			return _mm256_movemask_pd(_mm256_castsi256_pd(m)) != 0;
		}
		CCM_ALWAYS_INLINE static detail::SimdSizeType popcount(MaskMember m)
		{
			std::array<std::int64_t, 4> a;
			detail::si256_store(m, a.data());
			detail::SimdSizeType c = 0;
			for (int i = 0; i < 4; ++i)
			{
				c += (a[i] != 0) ? 1 : 0;
			}
			return c;
		}
		CCM_ALWAYS_INLINE static SimdMember select(MaskMember m, SimdMember a, SimdMember b)
		{
			return _mm256_blendv_pd(b, a, _mm256_castsi256_pd(m));
		}

		CCM_ALWAYS_INLINE static SimdMember op_sqrt(SimdMember v)
		{
			return _mm256_sqrt_pd(v);
		}
		CCM_ALWAYS_INLINE static SimdMember op_fabs(SimdMember v)
		{
			return _mm256_andnot_pd(_mm256_set1_pd(-0.0), v);
		}
		CCM_ALWAYS_INLINE static SimdMember op_min(SimdMember a, SimdMember b)
		{
			return _mm256_min_pd(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember op_max(SimdMember a, SimdMember b)
		{
			return _mm256_max_pd(a, b);
		}
		#define CCM_PP_MSVC_F64X4_ROUND(NAME, SFN)                                                                                                             \
			CCM_ALWAYS_INLINE static SimdMember NAME(SimdMember v)                                                                                             \
			{                                                                                                                                                  \
				std::array<double, 4> a;                                                                                                                       \
				_mm256_storeu_pd(a.data(), v);                                                                                                                 \
				for (int i = 0; i < 4; ++i)                                                                                                                    \
				{                                                                                                                                              \
					a[i] = detail::SFN<double>(a[i]);                                                                                                          \
				}                                                                                                                                              \
				return _mm256_loadu_pd(a.data());                                                                                                              \
			}
		CCM_PP_MSVC_F64X4_ROUND(op_floor, s_floor)
		CCM_PP_MSVC_F64X4_ROUND(op_ceil, s_ceil)
		CCM_PP_MSVC_F64X4_ROUND(op_trunc, s_trunc)
		CCM_PP_MSVC_F64X4_ROUND(op_round, s_round)
		#undef CCM_PP_MSVC_F64X4_ROUND
		CCM_ALWAYS_INLINE static SimdMember op_fma(SimdMember a, SimdMember b, SimdMember c)
		{
		#if CCMATH_SIMD_HAVE_FMA
			return _mm256_fmadd_pd(a, b, c);
		#else
			std::array<double, 4> aa, bb, cc;
			_mm256_storeu_pd(aa.data(), a);
			_mm256_storeu_pd(bb.data(), b);
			_mm256_storeu_pd(cc.data(), c);
			for (int i = 0; i < 4; ++i)
			{
				aa[i] = detail::s_fma<double>(aa[i], bb[i], cc[i]);
			}
			return _mm256_loadu_pd(aa.data());
		#endif
		}
		CCM_ALWAYS_INLINE static double hadd(SimdMember v)
		{
			std::array<double, 4> a;
			_mm256_storeu_pd(a.data(), v);
			return a[0] + a[1] + a[2] + a[3];
		}
		CCM_ALWAYS_INLINE static double hmin(SimdMember v)
		{
			std::array<double, 4> a;
			_mm256_storeu_pd(a.data(), v);
			double s = a[0];
			for (int i = 1; i < 4; ++i)
			{
				s = a[i] < s ? a[i] : s;
			}
			return s;
		}
		CCM_ALWAYS_INLINE static double hmax(SimdMember v)
		{
			std::array<double, 4> a;
			_mm256_storeu_pd(a.data(), v);
			double s = a[0];
			for (int i = 1; i < 4; ++i)
			{
				s = a[i] > s ? a[i] : s;
			}
			return s;
		}
		template <typename U> CCM_ALWAYS_INLINE static typename SimdTraits<U, VecAbi<4>>::SimdMember convert(SimdMember v)
		{
			std::array<double, 4> a;
			_mm256_storeu_pd(a.data(), v);
			typename SimdTraits<U, VecAbi<4>>::SimdMember r;
			for (detail::SimdSizeType i = 0; i < 4; ++i)
			{
				SimdTraits<U, VecAbi<4>>::set(r, i, static_cast<U>(a[i]));
			}
			return r;
		}
		template <typename U> CCM_ALWAYS_INLINE static typename SimdTraits<U, VecAbi<4>>::SimdMember bitcast(SimdMember v)
		{
			typename SimdTraits<U, VecAbi<4>>::SimdMember r;
			std::memcpy(&r, &v, sizeof(r));
			return r;
		}
	};

	template <> struct SimdTraits<std::int32_t, VecAbi<8>>
	{
		using value_type						   = std::int32_t;
		using MaskInt							   = std::int32_t;
		using SimdMember						   = __m256i;
		using MaskMember						   = __m256i;
		static constexpr detail::SimdSizeType size = 8;
		static constexpr std::size_t simd_align	   = 32;
		static constexpr std::size_t mask_align	   = 32;

		CCM_ALWAYS_INLINE static SimdMember broadcast(std::int32_t v)
		{
			return _mm256_set1_epi32(v);
		}
		CCM_ALWAYS_INLINE static std::int32_t get(SimdMember const & m, detail::SimdSizeType i)
		{
			std::array<std::int32_t, 8> a;
			detail::si256_store(m, a.data());
			return a[i];
		}
		CCM_ALWAYS_INLINE static void set(SimdMember & m, detail::SimdSizeType i, std::int32_t v)
		{
			std::array<std::int32_t, 8> a;
			detail::si256_store(m, a.data());
			a[i] = v;
			m	 = detail::si256_load(a.data());
		}
		CCM_ALWAYS_INLINE static SimdMember load(std::int32_t const * p)
		{
			return detail::si256_load(p);
		}
		CCM_ALWAYS_INLINE static void store(SimdMember const & m, std::int32_t * p)
		{
			detail::si256_store(m, p);
		}

		#if CCMATH_SIMD_HAVE_AVX2
		CCM_ALWAYS_INLINE static SimdMember add(SimdMember a, SimdMember b)
		{
			return _mm256_add_epi32(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember sub(SimdMember a, SimdMember b)
		{
			return _mm256_sub_epi32(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember negate(SimdMember a)
		{
			return _mm256_sub_epi32(_mm256_setzero_si256(), a);
		}
		CCM_ALWAYS_INLINE static MaskMember eq(SimdMember a, SimdMember b)
		{
			return _mm256_cmpeq_epi32(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember gt(SimdMember a, SimdMember b)
		{
			return _mm256_cmpgt_epi32(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember lt(SimdMember a, SimdMember b)
		{
			return _mm256_cmpgt_epi32(b, a);
		}
		#else
			#define CCM_PP_I32X8_SPILL2(NAME, OP)                                                                                                              \
				CCM_ALWAYS_INLINE static SimdMember NAME(SimdMember a, SimdMember b)                                                                           \
				{                                                                                                                                              \
					std::array<std::int32_t, 8> x, y;                                                                                                          \
					detail::si256_store(a, x.data());                                                                                                          \
					detail::si256_store(b, y.data());                                                                                                          \
					for (int i = 0; i < 8; ++i)                                                                                                                \
					{                                                                                                                                          \
						x[i] = static_cast<std::int32_t>(x[i] OP y[i]);                                                                                        \
					}                                                                                                                                          \
					return detail::si256_load(x.data());                                                                                                       \
				}
		CCM_PP_I32X8_SPILL2(add, +)
		CCM_PP_I32X8_SPILL2(sub, -)
			#undef CCM_PP_I32X8_SPILL2
		CCM_ALWAYS_INLINE static SimdMember negate(SimdMember a)
		{
			std::array<std::int32_t, 8> x;
			detail::si256_store(a, x.data());
			for (int i = 0; i < 8; ++i)
			{
				x[i] = static_cast<std::int32_t>(-x[i]);
			}
			return detail::si256_load(x.data());
		}
			#define CCM_PP_I32X8_SPILLCMP(NAME, OP)                                                                                                            \
				CCM_ALWAYS_INLINE static MaskMember NAME(SimdMember a, SimdMember b)                                                                           \
				{                                                                                                                                              \
					std::array<std::int32_t, 8> x, y, r;                                                                                                       \
					detail::si256_store(a, x.data());                                                                                                          \
					detail::si256_store(b, y.data());                                                                                                          \
					for (int i = 0; i < 8; ++i)                                                                                                                \
					{                                                                                                                                          \
						r[i] = (x[i] OP y[i]) ? -1 : 0;                                                                                                        \
					}                                                                                                                                          \
					return detail::si256_load(r.data());                                                                                                       \
				}
		CCM_PP_I32X8_SPILLCMP(eq, ==)
		CCM_PP_I32X8_SPILLCMP(gt, >)
		CCM_PP_I32X8_SPILLCMP(lt, <)
			#undef CCM_PP_I32X8_SPILLCMP
		#endif
		CCM_ALWAYS_INLINE static MaskMember ne(SimdMember a, SimdMember b)
		{
			return mnot(eq(a, b));
		}
		CCM_ALWAYS_INLINE static MaskMember le(SimdMember a, SimdMember b)
		{
			return mnot(gt(a, b));
		}
		CCM_ALWAYS_INLINE static MaskMember ge(SimdMember a, SimdMember b)
		{
			return mnot(lt(a, b));
		}

		CCM_ALWAYS_INLINE static MaskMember mbroadcast(bool b)
		{
			return _mm256_set1_epi32(b ? -1 : 0);
		}
		CCM_ALWAYS_INLINE static bool mget(MaskMember const & m, detail::SimdSizeType i)
		{
			return get(m, i) != 0;
		}
		CCM_ALWAYS_INLINE static void mset(MaskMember & m, detail::SimdSizeType i, bool b)
		{
			set(m, i, b ? -1 : 0);
		}
		CCM_ALWAYS_INLINE static MaskMember mand(MaskMember a, MaskMember b)
		{
			return detail::avx_and(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mor(MaskMember a, MaskMember b)
		{
			return detail::avx_or(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mxor(MaskMember a, MaskMember b)
		{
			return detail::avx_xor(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mnot(MaskMember a)
		{
			return detail::avx_not(a);
		}
		CCM_ALWAYS_INLINE static bool all_of(MaskMember m)
		{
			return _mm256_movemask_ps(_mm256_castsi256_ps(m)) == 0xFF;
		}
		CCM_ALWAYS_INLINE static bool any_of(MaskMember m)
		{
			return _mm256_movemask_ps(_mm256_castsi256_ps(m)) != 0;
		}
		CCM_ALWAYS_INLINE static detail::SimdSizeType popcount(MaskMember m)
		{
			std::array<std::int32_t, 8> a;
			detail::si256_store(m, a.data());
			detail::SimdSizeType c = 0;
			for (int i = 0; i < 8; ++i)
			{
				c += (a[i] != 0) ? 1 : 0;
			}
			return c;
		}
		CCM_ALWAYS_INLINE static SimdMember select(MaskMember m, SimdMember a, SimdMember b)
		{
			return _mm256_castps_si256(_mm256_blendv_ps(_mm256_castsi256_ps(b), _mm256_castsi256_ps(a), _mm256_castsi256_ps(m)));
		}

		CCM_ALWAYS_INLINE static SimdMember band(SimdMember a, SimdMember b)
		{
			return detail::avx_and(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember bor(SimdMember a, SimdMember b)
		{
			return detail::avx_or(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember bxor(SimdMember a, SimdMember b)
		{
			return detail::avx_xor(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember bnot(SimdMember a)
		{
			return detail::avx_not(a);
		}
		#define CCM_PP_I32X8_SPILLBIN(NAME, OP)                                                                                                                \
			CCM_ALWAYS_INLINE static SimdMember NAME(SimdMember a, SimdMember b)                                                                               \
			{                                                                                                                                                  \
				std::array<std::int32_t, 8> x, y;                                                                                                              \
				detail::si256_store(a, x.data());                                                                                                              \
				detail::si256_store(b, y.data());                                                                                                              \
				for (int i = 0; i < 8; ++i)                                                                                                                    \
				{                                                                                                                                              \
					x[i] = static_cast<std::int32_t>(x[i] OP y[i]);                                                                                            \
				}                                                                                                                                              \
				return detail::si256_load(x.data());                                                                                                           \
			}
		CCM_PP_I32X8_SPILLBIN(mul, *)
		CCM_PP_I32X8_SPILLBIN(div, /)
		CCM_PP_I32X8_SPILLBIN(mod, %)
		CCM_PP_I32X8_SPILLBIN(shl, <<)
		CCM_PP_I32X8_SPILLBIN(shr, >>)
		#undef CCM_PP_I32X8_SPILLBIN
		CCM_ALWAYS_INLINE static SimdMember op_min(SimdMember a, SimdMember b)
		{
			return select(lt(a, b), a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember op_max(SimdMember a, SimdMember b)
		{
			return select(gt(a, b), a, b);
		}
		CCM_ALWAYS_INLINE static std::int32_t hadd(SimdMember v)
		{
			std::array<std::int32_t, 8> a;
			detail::si256_store(v, a.data());
			std::int32_t s = 0;
			for (int i = 0; i < 8; ++i)
			{
				s = static_cast<std::int32_t>(s + a[i]);
			}
			return s;
		}
		CCM_ALWAYS_INLINE static std::int32_t hmin(SimdMember v)
		{
			std::array<std::int32_t, 8> a;
			detail::si256_store(v, a.data());
			std::int32_t s = a[0];
			for (int i = 1; i < 8; ++i)
			{
				s = a[i] < s ? a[i] : s;
			}
			return s;
		}
		CCM_ALWAYS_INLINE static std::int32_t hmax(SimdMember v)
		{
			std::array<std::int32_t, 8> a;
			detail::si256_store(v, a.data());
			std::int32_t s = a[0];
			for (int i = 1; i < 8; ++i)
			{
				s = a[i] > s ? a[i] : s;
			}
			return s;
		}
		template <typename U> CCM_ALWAYS_INLINE static typename SimdTraits<U, VecAbi<8>>::SimdMember convert(SimdMember v)
		{
			std::array<std::int32_t, 8> a;
			detail::si256_store(v, a.data());
			typename SimdTraits<U, VecAbi<8>>::SimdMember r;
			for (detail::SimdSizeType i = 0; i < 8; ++i)
			{
				SimdTraits<U, VecAbi<8>>::set(r, i, static_cast<U>(a[i]));
			}
			return r;
		}
		template <typename U> CCM_ALWAYS_INLINE static typename SimdTraits<U, VecAbi<8>>::SimdMember bitcast(SimdMember v)
		{
			typename SimdTraits<U, VecAbi<8>>::SimdMember r;
			std::memcpy(&r, &v, sizeof(r));
			return r;
		}
	};

	template <> struct SimdTraits<std::int64_t, VecAbi<4>>
	{
		using value_type						   = std::int64_t;
		using MaskInt							   = std::int64_t;
		using SimdMember						   = __m256i;
		using MaskMember						   = __m256i;
		static constexpr detail::SimdSizeType size = 4;
		static constexpr std::size_t simd_align	   = 32;
		static constexpr std::size_t mask_align	   = 32;

		CCM_ALWAYS_INLINE static SimdMember broadcast(std::int64_t v)
		{
			return _mm256_set1_epi64x(v);
		}
		CCM_ALWAYS_INLINE static std::int64_t get(SimdMember const & m, detail::SimdSizeType i)
		{
			std::array<std::int64_t, 4> a;
			detail::si256_store(m, a.data());
			return a[i];
		}
		CCM_ALWAYS_INLINE static void set(SimdMember & m, detail::SimdSizeType i, std::int64_t v)
		{
			std::array<std::int64_t, 4> a;
			detail::si256_store(m, a.data());
			a[i] = v;
			m	 = detail::si256_load(a.data());
		}
		CCM_ALWAYS_INLINE static SimdMember load(std::int64_t const * p)
		{
			return detail::si256_load(p);
		}
		CCM_ALWAYS_INLINE static void store(SimdMember const & m, std::int64_t * p)
		{
			detail::si256_store(m, p);
		}

		#if CCMATH_SIMD_HAVE_AVX2
		CCM_ALWAYS_INLINE static SimdMember add(SimdMember a, SimdMember b)
		{
			return _mm256_add_epi64(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember sub(SimdMember a, SimdMember b)
		{
			return _mm256_sub_epi64(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember negate(SimdMember a)
		{
			return _mm256_sub_epi64(_mm256_setzero_si256(), a);
		}
		CCM_ALWAYS_INLINE static MaskMember eq(SimdMember a, SimdMember b)
		{
			return _mm256_cmpeq_epi64(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember gt(SimdMember a, SimdMember b)
		{
			return _mm256_cmpgt_epi64(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember lt(SimdMember a, SimdMember b)
		{
			return _mm256_cmpgt_epi64(b, a);
		}
		#else
		CCM_ALWAYS_INLINE static SimdMember add(SimdMember a, SimdMember b)
		{
			std::array<std::int64_t, 4> x, y;
			detail::si256_store(a, x.data());
			detail::si256_store(b, y.data());
			for (int i = 0; i < 4; ++i)
			{
				x[i] = static_cast<std::int64_t>(x[i] + y[i]);
			}
			return detail::si256_load(x.data());
		}
		CCM_ALWAYS_INLINE static SimdMember sub(SimdMember a, SimdMember b)
		{
			std::array<std::int64_t, 4> x, y;
			detail::si256_store(a, x.data());
			detail::si256_store(b, y.data());
			for (int i = 0; i < 4; ++i)
			{
				x[i] = static_cast<std::int64_t>(x[i] - y[i]);
			}
			return detail::si256_load(x.data());
		}
		CCM_ALWAYS_INLINE static SimdMember negate(SimdMember a)
		{
			std::array<std::int64_t, 4> x;
			detail::si256_store(a, x.data());
			for (int i = 0; i < 4; ++i)
			{
				x[i] = static_cast<std::int64_t>(-x[i]);
			}
			return detail::si256_load(x.data());
		}
			#define CCM_PP_I64X4_SPILLCMP(NAME, OP)                                                                                                            \
				CCM_ALWAYS_INLINE static MaskMember NAME(SimdMember a, SimdMember b)                                                                           \
				{                                                                                                                                              \
					std::array<std::int64_t, 4> x, y, r;                                                                                                       \
					detail::si256_store(a, x.data());                                                                                                          \
					detail::si256_store(b, y.data());                                                                                                          \
					for (int i = 0; i < 4; ++i)                                                                                                                \
					{                                                                                                                                          \
						r[i] = (x[i] OP y[i]) ? -1 : 0;                                                                                                        \
					}                                                                                                                                          \
					return detail::si256_load(r.data());                                                                                                       \
				}
		CCM_PP_I64X4_SPILLCMP(eq, ==)
		CCM_PP_I64X4_SPILLCMP(gt, >)
		CCM_PP_I64X4_SPILLCMP(lt, <)
			#undef CCM_PP_I64X4_SPILLCMP
		#endif
		CCM_ALWAYS_INLINE static MaskMember ne(SimdMember a, SimdMember b)
		{
			return mnot(eq(a, b));
		}
		CCM_ALWAYS_INLINE static MaskMember le(SimdMember a, SimdMember b)
		{
			return mnot(gt(a, b));
		}
		CCM_ALWAYS_INLINE static MaskMember ge(SimdMember a, SimdMember b)
		{
			return mnot(lt(a, b));
		}

		CCM_ALWAYS_INLINE static MaskMember mbroadcast(bool b)
		{
			return _mm256_set1_epi64x(b ? -1 : 0);
		}
		CCM_ALWAYS_INLINE static bool mget(MaskMember const & m, detail::SimdSizeType i)
		{
			return get(m, i) != 0;
		}
		CCM_ALWAYS_INLINE static void mset(MaskMember & m, detail::SimdSizeType i, bool b)
		{
			set(m, i, b ? -1 : 0);
		}
		CCM_ALWAYS_INLINE static MaskMember mand(MaskMember a, MaskMember b)
		{
			return detail::avx_and(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mor(MaskMember a, MaskMember b)
		{
			return detail::avx_or(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mxor(MaskMember a, MaskMember b)
		{
			return detail::avx_xor(a, b);
		}
		CCM_ALWAYS_INLINE static MaskMember mnot(MaskMember a)
		{
			return detail::avx_not(a);
		}
		CCM_ALWAYS_INLINE static bool all_of(MaskMember m)
		{
			return _mm256_movemask_pd(_mm256_castsi256_pd(m)) == 0xF;
		}
		CCM_ALWAYS_INLINE static bool any_of(MaskMember m)
		{
			return _mm256_movemask_pd(_mm256_castsi256_pd(m)) != 0;
		}
		CCM_ALWAYS_INLINE static detail::SimdSizeType popcount(MaskMember m)
		{
			std::array<std::int64_t, 4> a;
			detail::si256_store(m, a.data());
			detail::SimdSizeType c = 0;
			for (int i = 0; i < 4; ++i)
			{
				c += (a[i] != 0) ? 1 : 0;
			}
			return c;
		}
		CCM_ALWAYS_INLINE static SimdMember select(MaskMember m, SimdMember a, SimdMember b)
		{
			return _mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(b), _mm256_castsi256_pd(a), _mm256_castsi256_pd(m)));
		}

		CCM_ALWAYS_INLINE static SimdMember band(SimdMember a, SimdMember b)
		{
			return detail::avx_and(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember bor(SimdMember a, SimdMember b)
		{
			return detail::avx_or(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember bxor(SimdMember a, SimdMember b)
		{
			return detail::avx_xor(a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember bnot(SimdMember a)
		{
			return detail::avx_not(a);
		}
		#define CCM_PP_I64X4_SPILLBIN(NAME, OP)                                                                                                                \
			CCM_ALWAYS_INLINE static SimdMember NAME(SimdMember a, SimdMember b)                                                                               \
			{                                                                                                                                                  \
				std::array<std::int64_t, 4> x, y;                                                                                                              \
				detail::si256_store(a, x.data());                                                                                                              \
				detail::si256_store(b, y.data());                                                                                                              \
				for (int i = 0; i < 4; ++i)                                                                                                                    \
				{                                                                                                                                              \
					x[i] = static_cast<std::int64_t>(x[i] OP y[i]);                                                                                            \
				}                                                                                                                                              \
				return detail::si256_load(x.data());                                                                                                           \
			}
		CCM_PP_I64X4_SPILLBIN(mul, *)
		CCM_PP_I64X4_SPILLBIN(div, /)
		CCM_PP_I64X4_SPILLBIN(mod, %)
		CCM_PP_I64X4_SPILLBIN(shl, <<)
		CCM_PP_I64X4_SPILLBIN(shr, >>)
		#undef CCM_PP_I64X4_SPILLBIN
		CCM_ALWAYS_INLINE static SimdMember op_min(SimdMember a, SimdMember b)
		{
			return select(lt(a, b), a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember op_max(SimdMember a, SimdMember b)
		{
			return select(gt(a, b), a, b);
		}
		CCM_ALWAYS_INLINE static std::int64_t hadd(SimdMember v)
		{
			std::array<std::int64_t, 4> a;
			detail::si256_store(v, a.data());
			return static_cast<std::int64_t>(a[0] + a[1] + a[2] + a[3]);
		}
		CCM_ALWAYS_INLINE static std::int64_t hmin(SimdMember v)
		{
			std::array<std::int64_t, 4> a;
			detail::si256_store(v, a.data());
			std::int64_t s = a[0];
			for (int i = 1; i < 4; ++i)
			{
				s = a[i] < s ? a[i] : s;
			}
			return s;
		}
		CCM_ALWAYS_INLINE static std::int64_t hmax(SimdMember v)
		{
			std::array<std::int64_t, 4> a;
			detail::si256_store(v, a.data());
			std::int64_t s = a[0];
			for (int i = 1; i < 4; ++i)
			{
				s = a[i] > s ? a[i] : s;
			}
			return s;
		}
		template <typename U> CCM_ALWAYS_INLINE static typename SimdTraits<U, VecAbi<4>>::SimdMember convert(SimdMember v)
		{
			std::array<std::int64_t, 4> a;
			detail::si256_store(v, a.data());
			typename SimdTraits<U, VecAbi<4>>::SimdMember r;
			for (detail::SimdSizeType i = 0; i < 4; ++i)
			{
				SimdTraits<U, VecAbi<4>>::set(r, i, static_cast<U>(a[i]));
			}
			return r;
		}
		template <typename U> CCM_ALWAYS_INLINE static typename SimdTraits<U, VecAbi<4>>::SimdMember bitcast(SimdMember v)
		{
			typename SimdTraits<U, VecAbi<4>>::SimdMember r;
			std::memcpy(&r, &v, sizeof(r));
			return r;
		}
	};
	#endif // CCMATH_SIMD_HAVE_AVX
} // namespace ccm::pp

#endif // CCM_PP_USE_MSVC_INTRIN
