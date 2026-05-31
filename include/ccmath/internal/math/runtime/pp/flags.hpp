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
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/floating_point_traits.hpp"

// ReSharper disable once CppUnusedIncludeDirective
#include "assume_aligned.hpp"
#include "fwddecl.hpp"
#include "simd_config.hpp"

namespace ccm::pp
{
	namespace detail
	{
		struct LoadStoreTag
		{
		};

		struct Convert : LoadStoreTag
		{
		};

		template <typename T>
		struct ConvertTo : LoadStoreTag
		{
			using type = T;
		};

		struct Aligned : LoadStoreTag
		{
			template <typename T, typename U>
			CCM_SIMD_INTRINSIC static constexpr U *adjust_pointer(U *ptr)
			{
				return static_cast<U *>(pp::assume_aligned<simd_alignment_v<T, U>>(ptr));
			}
		};

		template <std::size_t N>
		struct Overaligned : LoadStoreTag
		{
			static_assert(support::has_single_bit(N));

			template <typename, typename U>
			CCM_SIMD_INTRINSIC static constexpr U *adjust_pointer(U *ptr)
			{
				return static_cast<U *>(pp::assume_aligned<N>(ptr));
			}
		};

		struct Streaming : LoadStoreTag
		{
		};

		template <int L1, int L2>
		struct Prefetch : LoadStoreTag
		{
			template <typename, typename U>
			CCM_ALWAYS_INLINE static U *adjust_pointer(U *ptr)
			{
				// one read: 0, 0
				// L1: 0, 1
				// L2: 0, 2
				// L3: 0, 3
				// (exclusive cache line) for writing: 1, 0 / 1, 1
				/*          constexpr int write = 1;
							constexpr int level = 0-3;
				__builtin_prefetch(ptr, write, level)
				_mm_prefetch(reinterpret_cast<char const*>(ptr), _MM_HINT_T0);
				_mm_prefetch(reinterpret_cast<char const*>(ptr), _MM_HINT_T1);
				_mm_prefetch(reinterpret_cast<char const*>(ptr), _MM_HINT_T2);
				_mm_prefetch(reinterpret_cast<char const*>(ptr), _MM_HINT_ET0);
				_mm_prefetch(reinterpret_cast<char const*>(ptr), _MM_HINT_ET1);
				_mm_prefetch(reinterpret_cast<char const*>(ptr), _MM_HINT_NTA);*/
				return ptr;
			}
		};

		template <typename T>
		using is_loadstore_tag = std::is_base_of<LoadStoreTag, T>;
	} // namespace detail

	template <typename... Flags>
	struct simd_flags
	{
		static_assert((detail::is_loadstore_tag<Flags>::value && ...), "All Flags must derive from LoadStoreTag");

		// ReSharper disable once CppMemberFunctionMayBeStatic
		CCM_CONSTEVAL bool is_equal(simd_flags) const { return true; }

		template <typename... Other>
		CCM_CONSTEVAL bool is_equal([[maybe_unused]] simd_flags<Other...> other) const
		{
			return std::is_same_v<simd_flags<>, decltype(xor_flags(other))>;
		}

		template <typename... Other>
		CCM_CONSTEVAL bool test(simd_flags<Other...> other) const noexcept
		{
			return other.is_equal(and_flags(other));
		}

		friend CCM_CONSTEVAL auto operator|(simd_flags, simd_flags<>) { return simd_flags{}; }

		template <typename T0, typename... More>
		friend CCM_CONSTEVAL auto operator|(simd_flags, simd_flags<T0, More...>)
		{
			if constexpr ((std::is_same_v<Flags, T0> || ...)) { return simd_flags<Flags...>{} | simd_flags<More...>{}; }
			else { return simd_flags<Flags..., T0>{} | simd_flags<More...>{}; }
		}

		// ReSharper disable once CppMemberFunctionMayBeStatic
		CCM_CONSTEVAL auto and_flags(simd_flags<>) const { return simd_flags<>{}; }

		template <typename T0, typename... More>
		CCM_CONSTEVAL auto and_flags(simd_flags<T0, More...>) const
		{
			if constexpr ((std::is_same_v<Flags, T0> || ...)) { return simd_flags<T0>{} | (simd_flags{}.and_flags(simd_flags<More...>{})); }
			else { return simd_flags{}.and_flags(simd_flags<More...>{}); }
		}

		CCM_CONSTEVAL auto xor_flags(simd_flags<>) const { return simd_flags{}; }

		template <typename T0, typename... More>
		CCM_CONSTEVAL auto xor_flags(simd_flags<T0, More...>) const
		{
			if constexpr ((std::is_same_v<Flags, T0> || ...))
			{
				constexpr auto removed = (std::conditional_t<std::is_same_v<Flags, T0>, simd_flags<>, simd_flags<Flags>>{} | ...);
				return removed.xor_flags(simd_flags<More...>{});
			}
			else { return (simd_flags{} | simd_flags<T0>{}).xor_flags(simd_flags<More...>{}); }
		}

		template <typename F0, typename Tp, typename Ptr>
		static constexpr void apply_adjust_pointer(Ptr &ptr)
		{
			if constexpr (std::is_same_v<decltype(F0::template adjust_pointer<Tp>(ptr)), void>) { ptr = F0::template adjust_pointer<Tp>(ptr); }
		}

		template <typename Tp, typename Up>
		static constexpr Up *adjust_pointer(Up *ptr)
		{
			(apply_adjust_pointer<Flags, Tp>(ptr), ...);
			return ptr;
		}
	};

	// [simd.flags]
	inline constexpr simd_flags<> simd_flag_default;

	inline constexpr simd_flags<detail::Convert> simd_flag_convert;

	inline constexpr simd_flags<detail::Aligned> simd_flag_aligned;

	template <std::size_t N>
	requires(support::has_single_bit(N)) inline constexpr simd_flags<detail::Overaligned<N>> simd_flag_overaligned;

	// extensions
	template <typename T>
	inline constexpr simd_flags<detail::ConvertTo<T>> simd_flag_convert_to;

	inline constexpr simd_flags<detail::Streaming> simd_flag_streaming;

	template <int L1, int L2>
	inline constexpr simd_flags<detail::Prefetch<L1, L2>> simd_flag_prefetch;

} // namespace ccm::pp