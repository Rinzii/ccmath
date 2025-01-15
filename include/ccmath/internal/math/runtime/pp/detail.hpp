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

#include "assert_unreachable.hpp"
#include "ccmath/internal/predef/attributes/always_inline.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "const_eval.hpp"
#include "constexpr_wrapper.hpp"
#include "debug_print.hpp"
#include "fwddecl.hpp"
#include "may_alias.hpp"
#include "trap.hpp"
#include "x86_detail.hpp"

#include <climits>
#include <cstdint>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility> // For std::move, std::declval

namespace ccm::pp::detail
{
	template <typename T>
	constexpr bool is_power2_minus_1(T x)
	{
		using SignedIntType = ccm::pp::make_signed_int_t<T>;
		SignedIntType y		= __builtin_bit_cast(SignedIntType, x);
		return y == -1 || ccm::support::has_single_bit(x + 1);
	}

	template <typename T>
	using may_alias CCM_ATTR_MAY_ALIAS = T;

	/**
	 * @internal
	 * Tag used for private init constructor of simd and simd_mask
	 */
	struct PrivateInit
	{
		explicit PrivateInit() = default;
	};

	inline constexpr PrivateInit private_init = PrivateInit{};

	template <typename T, typename F>
	CCM_SIMD_INTRINSIC static void bit_iteration(T mask, F &&func)
	{
		static_assert(sizeof(0ULL) >= sizeof(T));
		using ConditionalType = std::conditional_t<sizeof(T) <= sizeof(0u), unsigned, unsigned long long>;
		ConditionalType k	  = mask;
		while (k)
		{
			func(ccm::support::countr_zero(k));
			k &= (k - 1);
		}
	}

	template <size_t Np, bool Sanitized, typename F>
	CCM_SIMD_INTRINSIC static void bit_iteration(BitMask<Np, Sanitized> mask, F &&func)
	{
		bit_iteration(mask.sanitized().to_bits(), func);
	}

#ifdef math_errhandling
	// Check if math functions need to account for floating-point exceptions.
	// If math_errhandling expands to an extern symbol, we must assume floating-point exceptions are relevant.
	template <int me = math_errhandling>
	CCM_CONSTEVAL bool handle_fpexcept_impl(int)
	{
		return me & MATH_ERREXCEPT;
	}

	CCM_CONSTEVAL bool handle_fpexcept_impl(float)
	{
		return true;
	}
#endif

	struct FloatingPointFlags
	{
		std::uint_least64_t handle_fp_exceptions : 1;
		std::uint_least64_t fast_math : 1;
		std::uint_least64_t finite_math_only : 1;
		std::uint_least64_t signed_zeros : 1;
		std::uint_least64_t reciprocal_math : 1;
		std::uint_least64_t math_errno : 1;
		std::uint_least64_t associative_math : 1;
		std::uint_least64_t float_eval_method : 2;

		// Constructor to initialize the bit-fields
		constexpr FloatingPointFlags()
			: handle_fp_exceptions(
#if defined(__NO_TRAPPING_MATH__) || defined(__FAST_MATH__) || defined(_M_FP_FAST) || defined(_M_FP_EXCEPT)
				  1
#elif defined(math_errhandling)
				  handle_fpexcept_impl(0)
#else
				  0
#endif
				  ),
			  fast_math(
#if defined(__FAST_MATH__) || defined(_M_FP_FAST)
				  1
#else
				  0
#endif
				  ),
			  finite_math_only(
#if defined(__FINITE_MATH_ONLY__) || defined(_M_FP_FAST)
				  1
#else
				  0
#endif
				  ),
			  signed_zeros(
#if defined(__NO_SIGNED_ZEROS__) || defined(_M_FP_FAST)
				  0
#elif defined(_M_FP_PRECISE) || defined(_M_FP_STRICT)
				  1
#else
				  1
#endif
				  ),
			  reciprocal_math(
#if defined(__RECIPROCAL_MATH__) || defined(_M_FP_FAST)
				  1
#else
				  0
#endif
				  ),
			  math_errno(
#if defined(__NO_MATH_ERRNO__)
				  0
#else
				  1
#endif
				  ),
			  associative_math(
#if defined(__ASSOCIATIVE_MATH__) || defined(_M_FP_FAST)
				  1
#else
				  0
#endif
				  ),
			  float_eval_method(
#if defined(__FLT_EVAL_METHOD__) && __FLT_EVAL_METHOD__ == 0
				  0
#elif defined(__FLT_EVAL_METHOD__) && __FLT_EVAL_METHOD__ == 1
				  1
#elif defined(__FLT_EVAL_METHOD__) && __FLT_EVAL_METHOD__ == 2
				  2
#elif defined(_M_FP_PRECISE)
				  0
#elif defined(_M_FP_STRICT)
				  1
#else
				  3
#endif
			  )
		{
		}
	};

	static_assert(sizeof(FloatingPointFlags) == sizeof(std::uint_least64_t));

	struct MachineFlags;

	template <typename... Flags>
	struct BuildFlags : Flags...
	{
	};

	using build_flags = BuildFlags<FloatingPointFlags, MachineFlags>;

	template <typename... Args>
	[[noreturn]] CCM_ALWAYS_INLINE void invoke_ub([[maybe_unused]] const char *msg, [[maybe_unused]] const Args &...args)
	{
#ifdef CCMATH_CONFIG_DEBUG_UB
		CCM_DEBUG_PRINT(msg, args...);
		CCM_DEBUG_PRINT("\n");
		CCM_TRAP();
#else
		__builtin_unreachable();
#endif
	}

	struct InvalidTraits
	{
		struct Unusable
		{
			Unusable()							  = delete;
			Unusable(const Unusable &)			  = delete;
			Unusable &operator=(const Unusable &) = delete;
			~Unusable()							  = delete;
		};

		template <typename>
		static constexpr bool explicit_mask_conversion = true;

		static constexpr int size		 = 0;
		static constexpr int full_size	 = 0;
		static constexpr bool is_partial = false;

		static constexpr size_t simd_align = 1;
		struct SimdImpl;
		using SimdMember = Unusable;
		struct SimdCastType;

		static constexpr size_t mask_align = 1;
		struct MaskImpl;
		using MaskMember = Unusable;
		struct MaskCastType;
	};
	// Primary template
	template <typename T, typename Abi, typename Flags = BuildFlags<FloatingPointFlags, MachineFlags>, typename Enable = void>
	struct SimdTraits : InvalidTraits
	{
	};

	// Specialization for when Abi::template IsValid<T>::value is true
	template <typename T, typename Abi, typename Flags>
	struct SimdTraits<T, Abi, Flags, std::enable_if_t<Abi::template IsValid<T>::value>> : Abi::template traits<T>
	{
	};

	// SimdMaskTraits using SimdTraits
	template <size_t B, typename Abi, typename Flags = BuildFlags<FloatingPointFlags, MachineFlags>>
	struct SimdMaskTraits : SimdTraits<mask_integer_from<B>, Abi, Flags>
	{
	};

	// Define a substitute for std::dynamic_extent
	constexpr std::size_t dynamic_extent = static_cast<std::size_t>(-1);

	template <typename Range>
	constexpr inline std::size_t static_range_size = []() -> std::size_t
	{
		using Type = std::remove_cv_t<std::remove_reference_t<Range>>;

		// Check for Type::size()
		if constexpr (std::is_invocable_v<decltype(&Type::size), Type>) { return Type::size(); }
		// Check for Type::extent
		else if constexpr (std::is_integral_v<decltype(Type::extent)>) { return Type::extent; }
		// Check for std::extent_v
		else if constexpr (std::extent_v<Type> > 0) { return std::extent_v<Type>; }
		// Check for std::tuple_size and tuple element compatibility
		else if constexpr (std::is_base_of_v<std::tuple_size<Type>, std::true_type>)
		{
			if constexpr (std::tuple_size_v<Type> >= 1 && std::is_same_v<std::tuple_element_t<0, Type>, typename Range::value_type>)
			{
				return std::tuple_size_v<Type>;
			}
		}
		// Default to dynamic extent
		return dynamic_extent;
	}();

	template <typename Integral, std::enable_if_t<std::is_integral_v<Integral>, int> = 0>
	CCM_SIMD_INTRINSIC SimdSizeType lowest_bit(Integral bits)
	{
		return ccm::support::ctz(bits);
	}

	template <typename Integral, std::enable_if_t<std::is_integral_v<Integral>, int> = 0>
	CCM_SIMD_INTRINSIC SimdSizeType highest_bit(Integral bits)
	{
		if constexpr (sizeof(bits) <= sizeof(int)) { return sizeof(int) * CHAR_BIT - 1 - ccm::support::countl_zero(bits); }
		else if constexpr (sizeof(bits) <= sizeof(long)) { return sizeof(long) * CHAR_BIT - 1 - ccm::support::countl_zero(bits); }
		else if constexpr (sizeof(bits) <= sizeof(long long)) { return sizeof(long long) * CHAR_BIT - 1 - ccm::support::countl_zero(bits); }
		else
		{
			pp::assert_unreachable<decltype(bits)>();
			return 0;
		}
	}
#include <type_traits>

	/**
	 * @internal
	 * Determines if one integral type has a higher rank than another.
	 *
	 * This checks if `From` is a strictly larger integral type than `To`, or if both types
	 * have the same size and `From` is the common type between the two.
	 *
	 * @tparam From The first integral type to compare.
	 * @tparam To The second integral type to compare.
	 */
	template <typename From, typename To>
	constexpr bool higher_integer_rank_than =
		std::is_integral_v<From> && std::is_integral_v<To> && (sizeof(From) > sizeof(To) || std::is_same_v<std::common_type_t<From, To>, From>);

	/**
	 * @internal
	 * Primary template for calculating a common type without integral promotions.
	 *
	 * This is the base template, which will be specialized for specific cases.
	 *
	 * @tparam T0 The first type.
	 * @tparam T1 The second type.
	 * @tparam Enable A SFINAE helper for specialization (default is `void`).
	 */
	template <typename T0, typename T1, typename Enable = void>
	struct nopromot_common_type;

	/**
	 * @internal
	 * Specialization for identical types.
	 *
	 * When both types are the same, the common type is simply the type itself.
	 *
	 * @tparam T The type (same for both inputs).
	 */
	template <typename T>
	struct nopromot_common_type<T, T>
	{
		using type = T;
	};

	/**
	 * @internal
	 * Specialization for different integral types with the same signedness.
	 *
	 * Chooses the type with the higher rank when both are signed or unsigned.
	 *
	 * @tparam T0 The first integral type.
	 * @tparam T1 The second integral type.
	 */
	template <typename T0, typename T1>
	struct nopromot_common_type<
		T0,
		T1,
		std::enable_if_t<std::is_integral_v<T0> && std::is_integral_v<T1> && !std::is_same_v<T0, T1> && (std::is_signed_v<T0> == std::is_signed_v<T1>)>>
	{
		using type = std::conditional_t<higher_integer_rank_than<T0, T1>, T0, T1>;
	};

	/**
	 * @internal
	 * Specialization for different integral types with different signedness.
	 *
	 * Determines the common type by comparing signedness and size, favoring
	 * the unsigned type if it can fully represent the signed type.
	 *
	 * @tparam T0 The first integral type.
	 * @tparam T1 The second integral type.
	 */
	template <typename T0, typename T1>
	struct nopromot_common_type<
		T0,
		T1,
		std::enable_if_t<std::is_integral_v<T0> && std::is_integral_v<T1> && !std::is_same_v<T0, T1> && (std::is_signed_v<T0> != std::is_signed_v<T1>)>>
	{
		using UnsignedType = std::conditional_t<std::is_signed_v<T0>, T1, T0>;
		using SignedType   = std::conditional_t<std::is_signed_v<T0>, T0, T1>;
		using type		   = std::conditional_t<(sizeof(UnsignedType) >= sizeof(SignedType)), UnsignedType, SignedType>;
	};

	/**
	 * @internal
	 * Fallback specialization for non-integral or incompatible types.
	 *
	 * Defaults to `std::common_type` for cases where the inputs are not integral,
	 * or where a suitable integral common type cannot be determined.
	 *
	 * @tparam T0 The first type.
	 * @tparam T1 The second type.
	 */
	template <typename T0, typename T1>
	struct nopromot_common_type<
		T0,
		T1,
		std::enable_if_t<!std::is_integral_v<T0> || !std::is_integral_v<T1> || (!higher_integer_rank_than<T0, T1> && std::is_same_v<T0, T1>)>>
		: std::common_type<T0, T1>
	{
	};

	/**
	 * @internal
	 * Alias template for `nopromot_common_type`.
	 *
	 * Provides a cleaner way to access the computed common type.
	 *
	 * @tparam T0 The first type.
	 * @tparam T1 The second type.
	 */
	template <typename T0, typename T1>
	using nopromot_common_type_t = typename nopromot_common_type<T0, T1>::type;

	template <typename Up, typename Accessor = Up, typename ValueType = typename Up::value_type>
	class SmartReference
	{
		friend Accessor;
		using SimdSizeType = std::size_t;

		SimdSizeType index;
		Up &obj;

		CCM_SIMD_INTRINSIC constexpr ValueType read() const noexcept { return Accessor::get(obj, index); }

		template <typename Tp>
		CCM_SIMD_INTRINSIC constexpr void write(Tp &&x) const
		{
			Accessor::set(obj, index, std::forward<Tp>(x));
		}

	public:
		CCM_SIMD_INTRINSIC constexpr SmartReference(Up &o, SimdSizeType i) noexcept : index(i), obj(o) {}

		using value_type = ValueType;

		SmartReference(const SmartReference &) = delete;

		CCM_SIMD_INTRINSIC constexpr operator value_type() const noexcept { return read(); }

		template <typename Tp>
		CCM_SIMD_INTRINSIC constexpr SmartReference operator=(Tp &&x) &&
		{
			write(std::forward<Tp>(x));
			return { obj, index };
		}

#define CCM_SIMD_OP(op)                                                                                                                                        \
	template <typename Tp>                                                                                                                                     \
	CCM_SIMD_INTRINSIC constexpr SmartReference operator op##=(Tp &&x) &&                                                                                      \
	{                                                                                                                                                          \
		const value_type &lhs = read();                                                                                                                        \
		write(lhs op std::forward<Tp>(x));                                                                                                                     \
		return { obj, index };                                                                                                                                 \
	}

		CCM_SIMD_OP(+)
		CCM_SIMD_OP(-)
		CCM_SIMD_OP(*)
		CCM_SIMD_OP(/)
		CCM_SIMD_OP(%)
		CCM_SIMD_OP(&)
		CCM_SIMD_OP(|)
		CCM_SIMD_OP(^)
		CCM_SIMD_OP(<<)
		CCM_SIMD_OP(>>)

#undef CCM_SIMD_OP

		template <typename Tp = void, typename = decltype(++std::declval<std::conditional_t<true, value_type, Tp> &>())>
		CCM_SIMD_INTRINSIC constexpr SmartReference operator++() &&
		{
			value_type x = read();
			write(++x);
			return { obj, index };
		}

		template <typename Tp = void, typename = decltype(std::declval<std::conditional_t<true, value_type, Tp> &>()++)>
		CCM_SIMD_INTRINSIC constexpr value_type operator++(int) &&
		{
			const value_type r = read();
			value_type x	   = r;
			write(++x);
			return r;
		}

		template <typename Tp = void, typename = decltype(--std::declval<std::conditional_t<true, value_type, Tp> &>())>
		CCM_SIMD_INTRINSIC constexpr SmartReference operator--() &&
		{
			value_type x = read();
			write(--x);
			return { obj, index };
		}

		template <typename Tp = void, typename = decltype(std::declval<std::conditional_t<true, value_type, Tp> &>()--)>
		CCM_SIMD_INTRINSIC constexpr value_type operator--(int) &&
		{
			const value_type r = read();
			value_type x	   = r;
			write(--x);
			return r;
		}

		CCM_SIMD_INTRINSIC friend constexpr void swap(SmartReference &&a, SmartReference &&b) noexcept(
			std::conjunction_v<std::is_nothrow_constructible<value_type, SmartReference &&>, std::is_nothrow_assignable<SmartReference &&, value_type &&>>)
		{
			value_type tmp					= std::forward<SmartReference>(a);
			std::forward<SmartReference>(a) = static_cast<value_type>(b);
			std::forward<SmartReference>(b) = std::move(tmp);
		}

		CCM_SIMD_INTRINSIC friend constexpr void
		swap(value_type &a, SmartReference &&b) noexcept(std::conjunction_v<std::is_nothrow_constructible<value_type, value_type &&>,
																			std::is_nothrow_assignable<value_type &, value_type &&>,
																			std::is_nothrow_assignable<SmartReference &&, value_type &&>>)
		{
			value_type tmp(std::move(a));
			a								= static_cast<value_type>(b);
			std::forward<SmartReference>(b) = std::move(tmp);
		}

		CCM_SIMD_INTRINSIC friend constexpr void swap(SmartReference &&a,
													  value_type &b) noexcept(std::conjunction_v<std::is_nothrow_constructible<value_type, SmartReference &&>,
																								 std::is_nothrow_assignable<value_type &, value_type &&>,
																								 std::is_nothrow_assignable<SmartReference &&, value_type &&>>)
		{
			value_type tmp(a);
			std::forward<SmartReference>(a) = std::move(b);
			b								= std::move(tmp);
		}
	};
} // namespace ccm::pp::detail

namespace ccm::pp
{

	/* C++20 version. Delete.
	template <detail::vectorizable T, detail::simd_abi_tag Abi>
		requires requires { Abi::size; }
	struct simd_size<T, Abi> : std::integral_constant<detail::SimdSizeType, Abi::size>
	{
	};
	*/

	// C++17 version - finish implementing
	template <typename T, detail::simd_abi_tag Abi, std::enable_if_t<detail::is_vectorizable<T>::value, int> = 0>
	struct simd_size<T, Abi> : std::integral_constant<detail::SimdSizeType, Abi::size>
	{
	};
} // namespace ccm::pp
