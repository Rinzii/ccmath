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

#include "ccmath/internal/predef/compiler_suppression/clang_compiler_suppression.hpp"
#include "ccmath/internal/predef/compiler_suppression/gcc_compiler_suppression.hpp"
#include "ccmath/internal/predef/compiler_suppression/msvc_compiler_suppression.hpp"
#include "ccmath/internal/predef/has_builtin.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/internal/support/type_traits.hpp"

#include <climits>
#include <limits>

// CCM_DISABLE_GCC_WARNING(-Wpedantic)

namespace ccm::support
{

	// Returns whether 'a + b' overflows, the result is stored in 'res'.
	template <typename T>
	[[nodiscard]] constexpr bool add_overflow(T a, T b, T & res)
	{
#if CCM_HAS_BUILTIN(__builtin_add_overflow)
		return __builtin_add_overflow(a, b, &res);
#else
		static_assert(std::is_integral_v<T>, "T must be an integral type");
		static_assert(!std::is_same_v<T, bool>, "T must not be a boolean type");
		static_assert(!std::is_enum_v<T>, "T must not be an enumerated type");

		using UnsignedT = std::make_unsigned_t<T>;
		const UnsignedT ua = static_cast<UnsignedT>(a);
		const UnsignedT ub = static_cast<UnsignedT>(b);
		const UnsignedT usum = ua + ub;

		if constexpr (std::is_unsigned_v<T>)
		{
			res = static_cast<T>(usum);
			return usum < ua;
		}
		else
		{
			constexpr UnsignedT SIGN_BIT = UnsignedT(1) << (std::numeric_limits<UnsignedT>::digits - 1);
			const bool overflow = ((~(ua ^ ub) & (ua ^ usum)) & SIGN_BIT) != 0;
			if (!overflow) { res = static_cast<T>(usum); }
			return overflow;
		}
#endif
	}

	// Returns whether 'a - b' overflows, the result is stored in 'res'.
	template <typename T>
	[[nodiscard]] constexpr bool sub_overflow(T a, T b, T & res)
	{
#if CCM_HAS_BUILTIN(__builtin_sub_overflow)
		return __builtin_sub_overflow(a, b, &res);
#else
		static_assert(std::is_integral_v<T>, "T must be an integral type");
		static_assert(!std::is_same_v<T, bool>, "T must not be a boolean type");
		static_assert(!std::is_enum_v<T>, "T must not be an enumerated type");

		using UnsignedT = std::make_unsigned_t<T>;
		const UnsignedT ua = static_cast<UnsignedT>(a);
		const UnsignedT ub = static_cast<UnsignedT>(b);
		const UnsignedT udiff = ua - ub;

		if constexpr (std::is_unsigned_v<T>)
		{
			res = static_cast<T>(udiff);
			return ua < ub;
		}
		else
		{
			constexpr UnsignedT SIGN_BIT = UnsignedT(1) << (std::numeric_limits<UnsignedT>::digits - 1);
			const bool overflow = (((ua ^ ub) & (ua ^ udiff)) & SIGN_BIT) != 0;
			if (!overflow) { res = static_cast<T>(udiff); }
			return overflow;
		}
#endif
	}

#define RETURN_IF(TYPE, BUILTIN)                                                                                                                               \
	if constexpr (std::is_same_v<T, TYPE>) { return BUILTIN(a, b, carry_in, &carry_out); }

	// Returns the result of 'a + b' taking into account 'carry_in'.
	// The carry out is stored in carry_out when provided, dropped otherwise.
	// We keep the pass by pointer interface for consistency with the intrinsic.
	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, T> add_with_carry(T a, T b, T carry_in, T & carry_out)
	{
		if constexpr (!is_constant_evaluated())
		{
#if CCM_HAS_BUILTIN(__builtin_addcb)
			RETURN_IF(unsigned char, __builtin_addcb)
#elif CCM_HAS_BUILTIN(__builtin_addcs)
			RETURN_IF(unsigned short, __builtin_addcs)
#elif CCM_HAS_BUILTIN(__builtin_addc)
			RETURN_IF(unsigned int, __builtin_addc)
#elif CCM_HAS_BUILTIN(__builtin_addcl)
			RETURN_IF(unsigned long, __builtin_addcl)
#elif CCM_HAS_BUILTIN(__builtin_addcll)
			RETURN_IF(unsigned long long, __builtin_addcll)
#endif
		}
		T sum	  = {};
		T carry1  = add_overflow(a, b, sum);
		T carry2  = add_overflow(sum, carry_in, sum);
		carry_out = carry1 | carry2;
		return sum;
	}

	// Returns the result of 'a - b' taking into account 'carry_in'.
	// The carry out is stored in carry_out when provided, dropped otherwise.
	// We keep the pass by pointer interface for consistency with the intrinsic.
	template <typename T>
	[[nodiscard]] inline constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, T> sub_with_borrow(T a, T b, T carry_in, T & carry_out)
	{
		if (!is_constant_evaluated())
		{
#if CCM_HAS_BUILTIN(__builtin_subcb)
			RETURN_IF(unsigned char, __builtin_subcb)
#elif CCM_HAS_BUILTIN(__builtin_subcs)
			RETURN_IF(unsigned short, __builtin_subcs)
#elif CCM_HAS_BUILTIN(__builtin_subc)
			RETURN_IF(unsigned int, __builtin_subc)
#elif CCM_HAS_BUILTIN(__builtin_subcl)
			RETURN_IF(unsigned long, __builtin_subcl)
#elif CCM_HAS_BUILTIN(__builtin_subcll)
			RETURN_IF(unsigned long long, __builtin_subcll)
#endif
		}
		T sub	  = {};
		T carry1  = sub_overflow(a, b, sub);
		T carry2  = sub_overflow(sub, carry_in, sub);
		carry_out = carry1 | carry2;
		return sub;
	}

#undef RETURN_IF

	// warning C4293: '>>': shift count negative or too big, undefined behavior
	// Normally MSVC would be correct but in this instance we assume that we know for a fact this undefined behavior will never happen.
	// Please be careful that UB is not introduced by using this function and that you know this
	// function will never be called with a count that is too big.
	CCM_DISABLE_MSVC_WARNING(4293)
	CCM_DISABLE_CLANG_WARNING(-Wshift-count-overflow) // Disabled for same reasons stated above

	// Rightmost count bits set to 1. Unsigned T only.
	template <typename T, std::size_t count>
	static constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, T> mask_trailing_ones()
	{
		constexpr unsigned T_BITS = CHAR_BIT * sizeof(T);
		static_assert(count <= T_BITS && "Invalid bit index");
		return count == 0 ? 0 : (T(-1) >> (T_BITS - count));
	}

	CCM_RESTORE_MSVC_WARNING()
	CCM_RESTORE_CLANG_WARNING()

	// Only unsigned types are allowed.
	template <typename T, std::size_t count>
	static constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, T> mask_leading_ones()
	{ return T(~mask_trailing_ones<T, CHAR_BIT * sizeof(T) - count>()); }

	// Only unsigned types are allowed.
	template <typename T, std::size_t count>
	static constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, T> mask_trailing_zeros()
	{ return mask_leading_ones<T, CHAR_BIT * sizeof(T) - count>(); }

	// Only unsigned types are allowed.
	template <typename T, std::size_t count>
	static constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, T> mask_leading_zeros()
	{ return mask_trailing_ones<T, CHAR_BIT * sizeof(T) - count>(); }

	// TODO(IanP): Review whether these helpers should replace or merge with exact_add and split in double_double.hpp.
	/**
	 * @brief Add a + b, such that &hi + &lo approximates a + b.
	 *
	 * Add a + b, such that *hi + *lo approximates a + b.
	 *  Assumes |a| >= |b|.
	 *  For rounding to nearest we have hi + lo = a + b exactly.
	 *  For directed rounding, we have
	 *  (a) hi + lo = a + b exactly when the exponent difference between a and b is at most 53 (the binary64 precision)
	 *  (b) otherwise |(a + b) - (hi + lo)| <= 2^-105 min(|a + b|, |hi|)
	 *
	 *  We also have |lo| < ulp(hi).
	 *
	 *	Algorithm Reference:
	 *	- https://hal.inria.fr/hal-03798376
	 *
	 * @param hi
	 * @param lo
	 * @param a
	 * @param b
	 */

	template <typename T, std::enable_if_t<traits::ccm_is_floating_point_v<T>, bool> = true>
	static constexpr void fast_two_sum(T & hi, T & lo, T a, T b)
	{
		hi		  = a + b;
		T const e = hi - a; // exact
		lo		  = b - e;	// exact
	}

	/* Algorithm 2 from https://hal.science/hal-01351529 */
	template <typename T, std::enable_if_t<traits::ccm_is_floating_point_v<T>, bool> = true>
	static constexpr void two_sum(T & s, T & t, T a, T b)
	{
		s				= a + b;
		T const a_prime = s - b;
		T const b_prime = s - a_prime;
		T const delta_a = a - a_prime;
		T const delta_b = b - b_prime;
		t				= delta_a + delta_b;
	}

	// Veltkamp's splitting: split x into xh + xl such that x = xh + xl exactly
	// xh fits in 32 bits and |xh| <= 2^e if 2^(e-1) <= |x| < 2^e
	// xl fits in 32 bits and |xl| < 2^(e-32)
	// See reference [1].
	static constexpr void veltkamp_split(long double & xh, long double & xl, long double x)
	{
		constexpr long double C = 0x1.00000001p+32L;
		long double const gamma = C * x;
		long double const delta = x - gamma;
		xh						= gamma + delta;
		xl						= x - xh;
	}

} // namespace ccm::support

// CCM_RESTORE_GCC_WARNING()
