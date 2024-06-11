/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/predef/compiler_suppression/msvc_compiler_suppression.hpp"
#include "ccmath/internal/predef/has_builtin.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/internal/support/type_traits.hpp"

#include <climits>

namespace ccm::support
{

	// Returns whether 'a + b' overflows, the result is stored in 'res'.
	template <typename T>
	[[nodiscard]] constexpr bool add_overflow(T a, T b, T & res)
	{
#if CCM_HAS_BUILTIN(__builtin_add_overflow)
		return __builtin_add_overflow(a, b, &res);
#else
		// This is a fallback implementation that should work on all compilers.
		static_assert(std::is_integral_v<T>, "T must be an integral type");
		static_assert(!std::is_same_v<T, bool>, "T must not be a boolean type");
		static_assert(!std::is_enum_v<T>, "T must not be an enumerated type");

		// Get the largest integral type that can hold the sum of a and b
		using LargerType = long long;
		auto la			 = static_cast<LargerType>(a);
		auto lb			 = static_cast<LargerType>(b);

		// Perform the addition
		LargerType const lres = la + lb;

		// Check for overflow by comparing the signs
		bool overflow = false;
		if (std::is_signed_v<T>)
		{
			if ((a > 0 && b > 0 && lres < 0) || (a < 0 && b < 0 && lres > 0)) { overflow = true; }
		}
		else
		{
			if (lres < 0 || lres > std::numeric_limits<T>::max()) { overflow = true; }
		}

		// Store the result if no overflow
		if (!overflow) { res = static_cast<T>(lres); }

		return overflow;
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

		// Get the largest integral type that can hold the sum of a and b
		using LargerType = long long;
		auto la			 = static_cast<LargerType>(a);
		auto lb			 = static_cast<LargerType>(b);

		// Perform the subtraction
		LargerType const lres = la - lb;

		// Check for overflow by comparing the signs
		bool overflow = false;
		if (std::is_signed_v<T>)
		{
			if ((b > 0 && a < std::numeric_limits<T>::min() + b) || (b < 0 && a > std::numeric_limits<T>::max() + b)) { overflow = true; }
		}
		else
		{
			if (a < b) { overflow = true; }
		}

		// Store the result if no overflow
		if (!overflow) { res = static_cast<T>(lres); }

		return overflow;
#endif
	}

#define RETURN_IF(TYPE, BUILTIN)                                                                                                                               \
	if constexpr (std::is_same_v<T, TYPE>) return BUILTIN(a, b, carry_in, carry_out);

	// Returns the result of 'a + b' taking into account 'carry_in'.
	// The carry out is stored in 'carry_out' it not 'nullptr', dropped otherwise.
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
	// The carry out is stored in 'carry_out' it not 'nullptr', dropped otherwise.
	// We keep the pass by pointer interface for consistency with the intrinsic.
	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, T> sub_with_borrow(T a, T b, T carry_in, T & carry_out)
	{
		if constexpr (!is_constant_evaluated())
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

	// Create a bitmask with the count right-most bits set to 1, and all other bits
	// set to 0.  Only unsigned types are allowed.
	template <typename T, std::size_t count>
	static constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, T> mask_trailing_ones()
	{
		constexpr unsigned T_BITS = CHAR_BIT * sizeof(T);
		static_assert(count <= T_BITS && "Invalid bit index");
		return count == 0 ? 0 : (T(-1) >> (T_BITS - count));
	}

	CCM_RESTORE_MSVC_WARNING()

	// Create a bitmask with the count left-most bits set to 1, and all other bits
	// set to 0.  Only unsigned types are allowed.
	template <typename T, std::size_t count>
	static constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, T> mask_leading_ones()
	{
		return T(~mask_trailing_ones<T, CHAR_BIT * sizeof(T) - count>());
	}

	// Create a bitmask with the count right-most bits set to 0, and all other bits
	// set to 1.  Only unsigned types are allowed.
	template <typename T, std::size_t count>
	static constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, T> mask_trailing_zeros()
	{
		return mask_leading_ones<T, CHAR_BIT * sizeof(T) - count>();
	}

	// Create a bitmask with the count left-most bits set to 0, and all other bits
	// set to 1.  Only unsigned types are allowed.
	template <typename T, std::size_t count>
	static constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, T> mask_leading_zeros()
	{
		return mask_trailing_ones<T, CHAR_BIT * sizeof(T) - count>();
	}

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

	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	static constexpr void fast_two_sum(T & hi, T & lo, T a, T b)
	{
		hi		  = a + b;
		T const e = hi - a; // exact
		lo		  = b - e;	// exact
	}

	/* Algorithm 2 from https://hal.science/hal-01351529 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
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