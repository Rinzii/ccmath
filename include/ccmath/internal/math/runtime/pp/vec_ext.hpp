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
#include "ccmath/internal/predef/compiler_suppression/gcc_compiler_suppression.hpp"

#include <array>
#include <cstddef>
#include <cstring>

// vec_ext backend: N lanes backed by a Clang/GCC vector extension
// (__attribute__((vector_size))), mirroring libc++'s vec_ext.h. The compiler
// lowers the vector ops to the target ISA (SSE / AVX / NEON). On toolchains
// without vector extensions (e.g. plain MSVC), or builds that define
// CCM_PP_FORCE_PORTABLE, it degrades to an array of scalars: correct, just not
// vectorized.

// Compilers that support the GNU/Clang vector extensions: GCC, Clang, Intel's
// modern oneAPI compiler (ICX, defines __clang__), and the NVIDIA HPC compiler
// (nvc++, defines __GNUC__). They are listed explicitly for clarity even though
// ICX/nvc++ already imply __clang__/__GNUC__. Anything else (in particular MSVC cl.exe)
// takes the intrinsic backend or the scalar array fallback.
#if !defined(CCM_PP_FORCE_ARRAY_BACKEND) && !defined(CCM_PP_FORCE_PORTABLE) && !defined(CCM_PP_FORCE_MSVC_INTRIN) &&                                           \
	(defined(__clang__) || defined(__GNUC__) || defined(__INTEL_LLVM_COMPILER) || defined(__NVCOMPILER))
	#define CCM_PP_HAS_VECTOR_EXT 1
#else
	#define CCM_PP_HAS_VECTOR_EXT 0
#endif

// Clang exposes __builtin_elementwise_* that lower a whole vector to one
// instruction (sqrtps, roundps, vfmadd, ...). GCC has no equivalent and falls
// back to a per-lane loop (which it still often vectorizes via SLP).
#if defined(__has_builtin)
	#define CCM_PP_HAS_EW(n) __has_builtin(__builtin_elementwise_##n)
#else
	#define CCM_PP_HAS_EW(n) 0
#endif

// Clang also exposes __builtin_reduce_* for horizontal reductions (log-N shuffle
// reductions). GCC falls back to a per-lane fold.
#if defined(__has_builtin)
	#define CCM_PP_HAS_RED(n) __has_builtin(__builtin_reduce_##n)
#else
	#define CCM_PP_HAS_RED(n) 0
#endif

namespace ccm::pp
{
#if CCM_PP_HAS_VECTOR_EXT
	// Wide GNU vector types use the AVX ABI on x86 when N*sizeof(T) > 16. TUs built
	// without -mavx still need VecAbi<N> to compile. Use CCM_DISABLE_GCC_WARNING /
	// CCM_RESTORE_GCC_WARNING from gcc_compiler_suppression.hpp (stacked push/pop).
	#if !defined(__AVX__)
	CCM_DISABLE_GCC_WARNING(-Wpsabi)
	#endif
	namespace detail
	{
		// A native vector of M elements of type U.
		template <typename U, int M> struct vec_builtin
		{
			typedef U type __attribute__((__vector_size__(sizeof(U) * M)));
		};
		template <typename U, int M> using vec_builtin_t = typename vec_builtin<U, M>::type;
	} // namespace detail

	template <typename T, detail::SimdSizeType N> struct SimdTraits<T, VecAbi<N>>
	{
		using value_type						   = T;
		using MaskInt							   = detail::mask_integer_from<sizeof(T)>;
		using SimdMember						   = detail::vec_builtin_t<T, N>;
		using MaskMember						   = detail::vec_builtin_t<MaskInt, N>;
		static constexpr detail::SimdSizeType size = N;
		static constexpr std::size_t simd_align	   = sizeof(SimdMember);
		static constexpr std::size_t mask_align	   = sizeof(MaskMember);

		CCM_ALWAYS_INLINE static SimdMember broadcast(T v)
		{
			SimdMember r;
			detail::unroll<N>([&](auto i) { r[i] = v; });
			return r;
		}
		CCM_ALWAYS_INLINE static T get(SimdMember const & m, detail::SimdSizeType i)
		{
			return m[i];
		}
		CCM_ALWAYS_INLINE static void set(SimdMember & m, detail::SimdSizeType i, T v)
		{
			m[i] = v;
		}
		// A single (unaligned) vector move rather than a per-lane loop.
		CCM_ALWAYS_INLINE static SimdMember load(T const * p)
		{
			SimdMember r;
			std::memcpy(&r, p, sizeof(r));
			return r;
		}
		CCM_ALWAYS_INLINE static void store(SimdMember const & m, T * p)
		{
			std::memcpy(p, &m, sizeof(m));
		}

		CCM_ALWAYS_INLINE static SimdMember add(SimdMember a, SimdMember b)
		{
			return a + b;
		}
		CCM_ALWAYS_INLINE static SimdMember sub(SimdMember a, SimdMember b)
		{
			return a - b;
		}
		CCM_ALWAYS_INLINE static SimdMember mul(SimdMember a, SimdMember b)
		{
			return a * b;
		}
		CCM_ALWAYS_INLINE static SimdMember div(SimdMember a, SimdMember b)
		{
			return a / b;
		}
		CCM_ALWAYS_INLINE static SimdMember negate(SimdMember a)
		{
			return -a;
		}

		// A same-size vector comparison yields a vector of the matching signed
		// integer with all-ones / zero lanes. The cast reinterprets it as our
		// canonical MaskMember type.
		CCM_ALWAYS_INLINE static MaskMember eq(SimdMember a, SimdMember b)
		{
			return reinterpret_mask(a == b);
		}
		CCM_ALWAYS_INLINE static MaskMember ne(SimdMember a, SimdMember b)
		{
			return reinterpret_mask(a != b);
		}
		CCM_ALWAYS_INLINE static MaskMember lt(SimdMember a, SimdMember b)
		{
			return reinterpret_mask(a < b);
		}
		CCM_ALWAYS_INLINE static MaskMember le(SimdMember a, SimdMember b)
		{
			return reinterpret_mask(a <= b);
		}
		CCM_ALWAYS_INLINE static MaskMember gt(SimdMember a, SimdMember b)
		{
			return reinterpret_mask(a > b);
		}
		CCM_ALWAYS_INLINE static MaskMember ge(SimdMember a, SimdMember b)
		{
			return reinterpret_mask(a >= b);
		}

		CCM_ALWAYS_INLINE static MaskMember mbroadcast(bool b)
		{
			MaskMember r;
			MaskInt const v = b ? static_cast<MaskInt>(-1) : MaskInt{ 0 };
			detail::unroll<N>([&](auto i) { r[i] = v; });
			return r;
		}
		CCM_ALWAYS_INLINE static bool mget(MaskMember const & m, detail::SimdSizeType i)
		{
			return m[i] != 0;
		}
		CCM_ALWAYS_INLINE static void mset(MaskMember & m, detail::SimdSizeType i, bool b)
		{
			m[i] = b ? static_cast<MaskInt>(-1) : MaskInt{ 0 };
		}
		CCM_ALWAYS_INLINE static MaskMember mand(MaskMember a, MaskMember b)
		{
			return a & b;
		}
		CCM_ALWAYS_INLINE static MaskMember mor(MaskMember a, MaskMember b)
		{
			return a | b;
		}
		CCM_ALWAYS_INLINE static MaskMember mxor(MaskMember a, MaskMember b)
		{
			return a ^ b;
		}
		CCM_ALWAYS_INLINE static MaskMember mnot(MaskMember a)
		{
			return ~a;
		}

		// All lanes are 0 or -1, so a horizontal AND/OR collapses to a movemask test
		// rather than a per-lane scalar chain.
		CCM_ALWAYS_INLINE static bool all_of(MaskMember m)
		{
	#if defined(__has_builtin) && __has_builtin(__builtin_reduce_and)
			return __builtin_reduce_and(m) != 0;
	#else
			bool r = true;
			detail::unroll<N>([&](auto i) { r = r && (m[i] != 0); });
			return r;
	#endif
		}
		CCM_ALWAYS_INLINE static bool any_of(MaskMember m)
		{
	#if defined(__has_builtin) && __has_builtin(__builtin_reduce_or)
			return __builtin_reduce_or(m) != 0;
	#else
			bool r = false;
			detail::unroll<N>([&](auto i) { r = r || (m[i] != 0); });
			return r;
	#endif
		}
		CCM_ALWAYS_INLINE static detail::SimdSizeType popcount(MaskMember m)
		{
			detail::SimdSizeType r = 0;
			detail::unroll<N>([&](auto i) { r += (m[i] != 0) ? 1 : 0; });
			return r;
		}

		// Branchless blend: pick a where the mask lane is set, else b. Reinterpret
		// to integer lanes so it lowers to a vector and/andnot/or (or a hardware
		// blend on SSE4.1+/AVX) instead of a scalar loop.
		CCM_ALWAYS_INLINE static SimdMember select(MaskMember m, SimdMember a, SimdMember b)
		{
			MaskMember const ai = reinterpret_cast<MaskMember>(a);
			MaskMember const bi = reinterpret_cast<MaskMember>(b);
			MaskMember const ri = (m & ai) | (~m & bi);
			return reinterpret_cast<SimdMember>(ri);
		}

		// Integer-only operations (instantiated only when used, i.e. for integral T).
		CCM_ALWAYS_INLINE static SimdMember mod(SimdMember a, SimdMember b)
		{
			return a % b;
		}
		CCM_ALWAYS_INLINE static SimdMember band(SimdMember a, SimdMember b)
		{
			return a & b;
		}
		CCM_ALWAYS_INLINE static SimdMember bor(SimdMember a, SimdMember b)
		{
			return a | b;
		}
		CCM_ALWAYS_INLINE static SimdMember bxor(SimdMember a, SimdMember b)
		{
			return a ^ b;
		}
		CCM_ALWAYS_INLINE static SimdMember bnot(SimdMember a)
		{
			return ~a;
		}
		CCM_ALWAYS_INLINE static SimdMember shl(SimdMember a, SimdMember b)
		{
			return a << b;
		}
		CCM_ALWAYS_INLINE static SimdMember shr(SimdMember a, SimdMember b)
		{
			return a >> b;
		}

		// Lane-type conversion (value) and bit reinterpret (same total size).
		template <typename U> CCM_ALWAYS_INLINE static detail::vec_builtin_t<U, N> convert(SimdMember v)
		{
	#if defined(__has_builtin) && __has_builtin(__builtin_convertvector)
			return __builtin_convertvector(v, detail::vec_builtin_t<U, N>);
	#else
			detail::vec_builtin_t<U, N> r;
			detail::unroll<N>([&](auto i) { r[i] = static_cast<U>(v[i]); });
			return r;
	#endif
		}
		template <typename U> CCM_ALWAYS_INLINE static detail::vec_builtin_t<U, N> bitcast(SimdMember v)
		{
			return reinterpret_cast<detail::vec_builtin_t<U, N>>(v);
		}

		// Math primitives. Clang lowers these to packed instructions. GCC takes the
		// per-lane fallback.
		CCM_ALWAYS_INLINE static SimdMember op_sqrt(SimdMember v)
		{
	#if CCM_PP_HAS_EW(sqrt)
			return __builtin_elementwise_sqrt(v);
	#else
			SimdMember r;
			detail::unroll<N>([&](auto i) { r[i] = detail::s_sqrt<T>(v[i]); });
			return r;
	#endif
		}
		CCM_ALWAYS_INLINE static SimdMember op_floor(SimdMember v)
		{
	#if CCM_PP_HAS_EW(floor)
			return __builtin_elementwise_floor(v);
	#else
			SimdMember r;
			detail::unroll<N>([&](auto i) { r[i] = detail::s_floor<T>(v[i]); });
			return r;
	#endif
		}
		CCM_ALWAYS_INLINE static SimdMember op_ceil(SimdMember v)
		{
	#if CCM_PP_HAS_EW(ceil)
			return __builtin_elementwise_ceil(v);
	#else
			SimdMember r;
			detail::unroll<N>([&](auto i) { r[i] = detail::s_ceil<T>(v[i]); });
			return r;
	#endif
		}
		CCM_ALWAYS_INLINE static SimdMember op_trunc(SimdMember v)
		{
	#if CCM_PP_HAS_EW(trunc)
			return __builtin_elementwise_trunc(v);
	#else
			SimdMember r;
			detail::unroll<N>([&](auto i) { r[i] = detail::s_trunc<T>(v[i]); });
			return r;
	#endif
		}
		CCM_ALWAYS_INLINE static SimdMember op_round(SimdMember v)
		{
	#if CCM_PP_HAS_EW(round)
			return __builtin_elementwise_round(v);
	#else
			SimdMember r;
			detail::unroll<N>([&](auto i) { r[i] = detail::s_round<T>(v[i]); });
			return r;
	#endif
		}
		CCM_ALWAYS_INLINE static SimdMember op_fabs(SimdMember v)
		{
	#if CCM_PP_HAS_EW(abs)
			return __builtin_elementwise_abs(v);
	#else
			SimdMember r;
			detail::unroll<N>([&](auto i) { r[i] = detail::s_fabs<T>(v[i]); });
			return r;
	#endif
		}
		CCM_ALWAYS_INLINE static SimdMember op_fma(SimdMember a, SimdMember b, SimdMember c)
		{
	#if CCM_PP_HAS_EW(fma)
			return __builtin_elementwise_fma(a, b, c);
	#else
			SimdMember r;
			detail::unroll<N>([&](auto i) { r[i] = detail::s_fma<T>(a[i], b[i], c[i]); });
			return r;
	#endif
		}
		CCM_ALWAYS_INLINE static SimdMember op_min(SimdMember a, SimdMember b)
		{
			return select(lt(a, b), a, b);
		}
		CCM_ALWAYS_INLINE static SimdMember op_max(SimdMember a, SimdMember b)
		{
			return select(lt(b, a), a, b);
		}

		// Horizontal reductions.
		CCM_ALWAYS_INLINE static T hadd(SimdMember v)
		{
			// __builtin_reduce_add is integer-only (float add is not associative);
			// floats use the per-lane fold.
	#if CCM_PP_HAS_RED(add)
			if constexpr (std::is_integral<T>::value)
			{
				return __builtin_reduce_add(v);
			} else
	#endif
			{
				T s = T(0);
				detail::unroll<N>([&](auto i) { s = static_cast<T>(s + v[i]); });
				return s;
			}
		}
		CCM_ALWAYS_INLINE static T hmin(SimdMember v)
		{
	#if CCM_PP_HAS_RED(min)
			return __builtin_reduce_min(v);
	#else
			T s = v[0];
			detail::unroll<N>([&](auto i) { s = v[i] < s ? v[i] : s; });
			return s;
	#endif
		}
		CCM_ALWAYS_INLINE static T hmax(SimdMember v)
		{
	#if CCM_PP_HAS_RED(max)
			return __builtin_reduce_max(v);
	#else
			T s = v[0];
			detail::unroll<N>([&](auto i) { s = s < v[i] ? v[i] : s; });
			return s;
	#endif
		}

	private:
		// Reinterpret an arbitrary same-size integer comparison vector as MaskMember.
		template <typename Cmp> CCM_ALWAYS_INLINE static MaskMember reinterpret_mask(Cmp c)
		{
			return reinterpret_cast<MaskMember>(c);
		}
	};
#else // CCM_PP_HAS_VECTOR_EXT == 0 : portable array fallback
	template <typename T, detail::SimdSizeType N> struct SimdTraits<T, VecAbi<N>>
	{
		using value_type						   = T;
		using MaskInt							   = detail::mask_integer_from<sizeof(T)>;
		using SimdMember						   = std::array<T, static_cast<std::size_t>(N)>;
		using MaskMember						   = std::array<MaskInt, static_cast<std::size_t>(N)>;
		static constexpr detail::SimdSizeType size = N;
		static constexpr std::size_t simd_align	   = alignof(SimdMember);
		static constexpr std::size_t mask_align	   = alignof(MaskMember);

		CCM_ALWAYS_INLINE static SimdMember broadcast(T v)
		{
			SimdMember r;
			detail::unroll<N>([&](auto i) { r[i] = v; });
			return r;
		}
		CCM_ALWAYS_INLINE static T get(SimdMember const & m, detail::SimdSizeType i)
		{
			return m[i];
		}
		CCM_ALWAYS_INLINE static void set(SimdMember & m, detail::SimdSizeType i, T v)
		{
			m[i] = v;
		}
		CCM_ALWAYS_INLINE static SimdMember load(T const * p)
		{
			SimdMember r;
			std::memcpy(r.data(), p, sizeof(r));
			return r;
		}
		CCM_ALWAYS_INLINE static void store(SimdMember const & m, T * p)
		{
			std::memcpy(p, m.data(), sizeof(m));
		}

	#define CCM_PP_ARRAY_BINOP(NAME, OP)                                                                                                                       \
		CCM_ALWAYS_INLINE static SimdMember NAME(SimdMember a, SimdMember b)                                                                                   \
		{                                                                                                                                                      \
			SimdMember r;                                                                                                                                      \
			detail::unroll<N>([&](auto i) { r[i] = a[i] OP b[i]; });                                                                                           \
			return r;                                                                                                                                          \
		}
		CCM_PP_ARRAY_BINOP(add, +)
		CCM_PP_ARRAY_BINOP(sub, -)
		CCM_PP_ARRAY_BINOP(mul, *)
		CCM_PP_ARRAY_BINOP(div, /)
	#undef CCM_PP_ARRAY_BINOP
		CCM_ALWAYS_INLINE static SimdMember negate(SimdMember a)
		{
			SimdMember r;
			detail::unroll<N>([&](auto i) { r[i] = -a[i]; });
			return r;
		}

	#define CCM_PP_ARRAY_CMP(NAME, OP)                                                                                                                         \
		CCM_ALWAYS_INLINE static MaskMember NAME(SimdMember a, SimdMember b)                                                                                   \
		{                                                                                                                                                      \
			MaskMember r;                                                                                                                                      \
			detail::unroll<N>([&](auto i) { r[i] = (a[i] OP b[i]) ? static_cast<MaskInt>(-1) : MaskInt{ 0 }; });                                               \
			return r;                                                                                                                                          \
		}
		CCM_PP_ARRAY_CMP(eq, ==)
		CCM_PP_ARRAY_CMP(ne, !=)
		CCM_PP_ARRAY_CMP(lt, <)
		CCM_PP_ARRAY_CMP(le, <=)
		CCM_PP_ARRAY_CMP(gt, >)
		CCM_PP_ARRAY_CMP(ge, >=)
	#undef CCM_PP_ARRAY_CMP

		CCM_ALWAYS_INLINE static MaskMember mbroadcast(bool b)
		{
			MaskMember r;
			MaskInt const v = b ? static_cast<MaskInt>(-1) : MaskInt{ 0 };
			detail::unroll<N>([&](auto i) { r[i] = v; });
			return r;
		}
		CCM_ALWAYS_INLINE static bool mget(MaskMember const & m, detail::SimdSizeType i)
		{
			return m[i] != 0;
		}
		CCM_ALWAYS_INLINE static void mset(MaskMember & m, detail::SimdSizeType i, bool b)
		{
			m[i] = b ? static_cast<MaskInt>(-1) : MaskInt{ 0 };
		}
		CCM_ALWAYS_INLINE static MaskMember mand(MaskMember a, MaskMember b)
		{
			MaskMember r;
			detail::unroll<N>([&](auto i) { r[i] = a[i] & b[i]; });
			return r;
		}
		CCM_ALWAYS_INLINE static MaskMember mor(MaskMember a, MaskMember b)
		{
			MaskMember r;
			detail::unroll<N>([&](auto i) { r[i] = a[i] | b[i]; });
			return r;
		}
		CCM_ALWAYS_INLINE static MaskMember mxor(MaskMember a, MaskMember b)
		{
			MaskMember r;
			detail::unroll<N>([&](auto i) { r[i] = a[i] ^ b[i]; });
			return r;
		}
		CCM_ALWAYS_INLINE static MaskMember mnot(MaskMember a)
		{
			MaskMember r;
			detail::unroll<N>([&](auto i) { r[i] = static_cast<MaskInt>(~a[i]); });
			return r;
		}

		CCM_ALWAYS_INLINE static bool all_of(MaskMember m)
		{
			bool r = true;
			detail::unroll<N>([&](auto i) { r = r && (m[i] != 0); });
			return r;
		}
		CCM_ALWAYS_INLINE static bool any_of(MaskMember m)
		{
			bool r = false;
			detail::unroll<N>([&](auto i) { r = r || (m[i] != 0); });
			return r;
		}
		CCM_ALWAYS_INLINE static detail::SimdSizeType popcount(MaskMember m)
		{
			detail::SimdSizeType r = 0;
			detail::unroll<N>([&](auto i) { r += (m[i] != 0) ? 1 : 0; });
			return r;
		}

		CCM_ALWAYS_INLINE static SimdMember select(MaskMember m, SimdMember a, SimdMember b)
		{
			SimdMember r;
			detail::unroll<N>([&](auto i) { r[i] = (m[i] != 0) ? a[i] : b[i]; });
			return r;
		}

	#define CCM_PP_ARRAY_BINOP(NAME, OP)                                                                                                                       \
		CCM_ALWAYS_INLINE static SimdMember NAME(SimdMember a, SimdMember b)                                                                                   \
		{                                                                                                                                                      \
			SimdMember r;                                                                                                                                      \
			detail::unroll<N>([&](auto i) { r[i] = static_cast<T>(a[i] OP b[i]); });                                                                           \
			return r;                                                                                                                                          \
		}
		CCM_PP_ARRAY_BINOP(mod, %)
		CCM_PP_ARRAY_BINOP(band, &)
		CCM_PP_ARRAY_BINOP(bor, |)
		CCM_PP_ARRAY_BINOP(bxor, ^)
		CCM_PP_ARRAY_BINOP(shl, <<)
		CCM_PP_ARRAY_BINOP(shr, >>)
	#undef CCM_PP_ARRAY_BINOP
		CCM_ALWAYS_INLINE static SimdMember bnot(SimdMember a)
		{
			SimdMember r;
			detail::unroll<N>([&](auto i) { r[i] = static_cast<T>(~a[i]); });
			return r;
		}

		template <typename U> CCM_ALWAYS_INLINE static std::array<U, static_cast<std::size_t>(N)> convert(SimdMember v)
		{
			std::array<U, static_cast<std::size_t>(N)> r;
			detail::unroll<N>([&](auto i) { r[i] = static_cast<U>(v[i]); });
			return r;
		}
		template <typename U> CCM_ALWAYS_INLINE static std::array<U, static_cast<std::size_t>(N)> bitcast(SimdMember v)
		{
			std::array<U, static_cast<std::size_t>(N)> r;
			std::memcpy(r.data(), v.data(), sizeof(r));
			return r;
		}

	#define CCM_PP_ARRAY_MATH1(NAME, SFN)                                                                                                                      \
		CCM_ALWAYS_INLINE static SimdMember NAME(SimdMember v)                                                                                                 \
		{                                                                                                                                                      \
			SimdMember r;                                                                                                                                      \
			detail::unroll<N>([&](auto i) { r[i] = detail::SFN<T>(v[i]); });                                                                                   \
			return r;                                                                                                                                          \
		}
		CCM_PP_ARRAY_MATH1(op_sqrt, s_sqrt)
		CCM_PP_ARRAY_MATH1(op_floor, s_floor)
		CCM_PP_ARRAY_MATH1(op_ceil, s_ceil)
		CCM_PP_ARRAY_MATH1(op_trunc, s_trunc)
		CCM_PP_ARRAY_MATH1(op_round, s_round)
		CCM_PP_ARRAY_MATH1(op_fabs, s_fabs)
	#undef CCM_PP_ARRAY_MATH1
		CCM_ALWAYS_INLINE static SimdMember op_fma(SimdMember a, SimdMember b, SimdMember c)
		{
			SimdMember r;
			detail::unroll<N>([&](auto i) { r[i] = detail::s_fma<T>(a[i], b[i], c[i]); });
			return r;
		}
		CCM_ALWAYS_INLINE static SimdMember op_min(SimdMember a, SimdMember b)
		{
			SimdMember r;
			detail::unroll<N>([&](auto i) { r[i] = a[i] < b[i] ? a[i] : b[i]; });
			return r;
		}
		CCM_ALWAYS_INLINE static SimdMember op_max(SimdMember a, SimdMember b)
		{
			SimdMember r;
			detail::unroll<N>([&](auto i) { r[i] = a[i] < b[i] ? b[i] : a[i]; });
			return r;
		}

		CCM_ALWAYS_INLINE static T hadd(SimdMember v)
		{
			T s = T(0);
			detail::unroll<N>([&](auto i) { s = static_cast<T>(s + v[i]); });
			return s;
		}
		CCM_ALWAYS_INLINE static T hmin(SimdMember v)
		{
			T s = v[0];
			detail::unroll<N>([&](auto i) { s = v[i] < s ? v[i] : s; });
			return s;
		}
		CCM_ALWAYS_INLINE static T hmax(SimdMember v)
		{
			T s = v[0];
			detail::unroll<N>([&](auto i) { s = s < v[i] ? v[i] : s; });
			return s;
		}
	};
	#if !defined(__AVX__)
	CCM_RESTORE_GCC_WARNING()
	#endif
#endif // CCM_PP_HAS_VECTOR_EXT
} // namespace ccm::pp

// When the vector extensions are unavailable, the MSVC SSE2 intrinsic backend
// overrides the array fallback for the 128-bit native widths. Included last so
// the array generic above is the visible base for everything it does not cover.
#include "ccmath/internal/math/runtime/pp/msvc_intrin.hpp"
