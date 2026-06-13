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

#include "ccmath/internal/predef/attributes/always_inline.hpp"

#include <cstddef>
#include <cstdint>
#include <type_traits>

// Load/store flags. The C++26 surface spells these as simd_flags<...> with the
// simd_flag_default / simd_flag_aligned / simd_flag_overaligned objects; the
// Parallelism TS spells the equivalents element_aligned / vector_aligned. Both
// names are provided. An aligned flag promises the pointer is suitably aligned,
// which is communicated to the optimizer (see flags_align_request /
// assume_aligned_ptr) so the load/store can use aligned moves and so a
// surrounding loop vectorizes more readily.

namespace ccm::pp
{
	namespace detail
	{
		struct flag_aligned
		{
		};
		template <std::size_t N>
		struct flag_overaligned
		{
		};
	} // namespace detail

	template <typename... Flags>
	struct simd_flags;

	namespace detail
	{
		template <typename F>
		struct overalign_value : std::integral_constant<std::size_t, 0>
		{
		};
		template <std::size_t N>
		struct overalign_value<flag_overaligned<N>> : std::integral_constant<std::size_t, N>
		{
		};

		// Alignment a flag set promises for a load/store:
		//   0  -> no promise (treat as unaligned)
		//   1  -> the simd object's natural alignment
		//   >1 -> an explicit overalignment N
		template <typename Flags>
		struct flags_align_request : std::integral_constant<std::size_t, 0>
		{
		};
		template <typename... Fs>
		struct flags_align_request<simd_flags<Fs...>>
			: std::integral_constant<std::size_t,
									 ((std::size_t{ 0 } + ... + overalign_value<Fs>::value) != 0)
										 ? (std::size_t{ 0 } + ... + overalign_value<Fs>::value)
										 : ((std::is_same<Fs, flag_aligned>::value || ...) ? std::size_t{ 1 } : std::size_t{ 0 })>
		{
		};

		// Communicate a known alignment to the optimizer. The pointer value is
		// unchanged; the compiler may then use aligned moves / assume alignment.
		template <std::size_t A, typename T>
		CCM_ALWAYS_INLINE T const *assume_aligned_ptr(T const *p)
		{
#if defined(__clang__) || defined(__GNUC__)
			return static_cast<T const *>(__builtin_assume_aligned(p, A));
#elif defined(_MSC_VER)
			__assume((reinterpret_cast<std::uintptr_t>(p) & (A - 1)) == 0);
			return p;
#else
			return p;
#endif
		}
		template <std::size_t A, typename T>
		CCM_ALWAYS_INLINE T *assume_aligned_ptr(T *p)
		{
#if defined(__clang__) || defined(__GNUC__)
			return static_cast<T *>(__builtin_assume_aligned(p, A));
#elif defined(_MSC_VER)
			__assume((reinterpret_cast<std::uintptr_t>(p) & (A - 1)) == 0);
			return p;
#else
			return p;
#endif
		}
	} // namespace detail

	template <typename... Flags>
	struct simd_flags
	{
	};

	template <typename T>
	struct is_simd_flag_type : std::false_type
	{
	};
	template <typename... Flags>
	struct is_simd_flag_type<simd_flags<Flags...>> : std::true_type
	{
	};
	template <typename T>
	inline constexpr bool is_simd_flag_type_v = is_simd_flag_type<T>::value;

	// [simd.flags]
	inline constexpr simd_flags<> simd_flag_default{};
	inline constexpr simd_flags<detail::flag_aligned> simd_flag_aligned{};
	template <std::size_t N>
	inline constexpr simd_flags<detail::flag_overaligned<N>> simd_flag_overaligned{};

	// Parallelism TS spellings.
	using element_aligned_tag = simd_flags<>;
	using vector_aligned_tag  = simd_flags<detail::flag_aligned>;
	inline constexpr element_aligned_tag element_aligned{};
	inline constexpr vector_aligned_tag vector_aligned{};
} // namespace ccm::pp
