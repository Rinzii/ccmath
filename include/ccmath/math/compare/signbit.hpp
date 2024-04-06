/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

// From Ian: Sorry for the macro gore. Kinda required to make this work in a constexpr context. :(
// TODO: Move all the macro gore into two files. One to define the macros and another to undefine them.

#pragma once

#include "ccmath/math/compare/isnan.hpp"

// Some compilers have __builtin_signbit which is constexpr for some compilers
#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_SIGNBIT
	// GCC 6.1+ has constexpr __builtin_signbit that DOES allow static_assert. Clang & MSVC do not.
	#if defined(__GNUC__) && (__GNUC__ >= 6 && __GNUC_MINOR__ >= 1)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_SIGNBIT
	#endif
#endif

#if !defined(CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN) && !defined(CCMATH_HAS_CONSTEXPR_BUILTIN_SIGNBIT)
	// GCC 6.1 has constexpr __builtin_copysign that DOES allow static_assert
	#if defined(__GNUC__) && (__GNUC__ >= 6 && __GNUC_MINOR__ >= 1)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
	#endif

	// Clang 5.0.0 has constexpr __builtin_copysign that DOES allow static_assert
	#if defined(__clang__) && (__clang_major__ >= 5)
		#define CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
	#endif
#endif

// We only implement this for MSVC as that is the only manner to get constexpr signbit that is also static_assert-able
#if !defined(CCMATH_HAS_BUILTIN_BIT_CAST) && !defined(CCMATH_HAS_CONSTEXPR_BUILTIN_SIGNBIT) && !defined(CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN)
	#if (defined(_MSC_VER) && _MSC_VER >= 1927)
		#define CCMATH_HAS_BUILTIN_BIT_CAST
		#include "ccmath/internal/support/floating_point_traits.hpp"
		#include <limits>  // for std::numeric_limits
		#include <cstdint> // for std::uint64_t
	#endif
#endif

namespace ccm
{
	/**
	 * @brief Detects the sign bit of zeroes, infinities, and NaNs.
	 * @tparam T A floating-point type.
	 * @param x A floating-point number.
	 * @return true if \p x is negative, false otherwise.
	 *
	 * @note This function has multiple implementations based on the compiler and the version of
	 * the the compiler used. With nearly all implementations, this function is fully constexpr and will return
	 * the same values as std::signbit along with being static_assert-able.
	 *
	 * @warning ccm::signbit currently is only ensured to work on little-endian systems. There is currently no guarantee this it will work on big-endian
	 * systems.
	 *
	 * @attention Implementing signbit is a non-trivial task and requires a lot of compiler magic to allow for ccm::signbit to be
	 * fully constexpr and static_assert-able while also conforming to the standard. CCMath has done its best to provide
	 * a constexpr signbit for all compilers, but we feel that not covering edge cases would be unacceptable and as such
	 * if your compiler is not supported a static assertion will be triggered. If this happens to you please report it to
	 * the dev team and we will try to bring support to your compiler ASAP if we are able to!
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
	[[nodiscard]] constexpr bool signbit(T x) noexcept
	{
#if defined(CCMATH_HAS_CONSTEXPR_BUILTIN_SIGNBIT)
		return __builtin_signbit(x);
#elif defined(CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN)
		// use __builtin_copysign to check for the sign of zero
		if (x == static_cast<T>(0) || ccm::isnan(x))
		{
			// If constexpr only works with gcc 7.1+. Without if constexpr we work till GCC 5.1+
			// This works with clang 5.0.0 no problem even with if constexpr
			if constexpr (std::is_same_v<T, float>) { return __builtin_copysignf(1.0f, x) < 0; }
			else if constexpr (std::is_same_v<T, double>) { return __builtin_copysign(1.0, x) < 0; }
			else if constexpr (std::is_same_v<T, long double>) { return __builtin_copysignl(1.0l, x) < 0; }

			return false;
		}

		return x < static_cast<T>(0);
#elif defined(CCMATH_HAS_BUILTIN_BIT_CAST)
		// Check for the sign of +0.0 and -0.0 with __builtin_bit_cast
		if (x == static_cast<T>(0) || ccm::isnan(x))
		{
			const auto bits = __builtin_bit_cast(helpers::float_bits_t<T>, x);
			return (bits & helpers::sign_mask_v<T>) != 0;
		}

		return x < static_cast<T>(0);
#else
		static_assert(false, "ccm::signbit is not implemented for this compiler. Please report this issue to the dev!");
		return false;
#endif
	}

	/**
	 * @brief Detects the sign bit of a number.
	 * @tparam T An integral type.
	 * @param x An integral number.
	 * @return true if \p x is negative, false otherwise.
	 *
	 * @note This function is constexpr and will return the same values as std::signbit along with being static_assert-able.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer> && std::is_signed_v<Integer>, int> = 0>
	[[nodiscard]] constexpr bool signbit(Integer x) noexcept
	{
		// There is no concept of -0 for integers. So we can just check if the number is less than 0.
		return x < 0;
	}

	/**
	 * @brief Detects the sign bit of a number.
	 * @tparam T An integral type.
	 * @param x An integral number.
	 * @return false as an unsigned number can't be negative.
	 *
	 * @note This function is constexpr and will return the same values as std::signbit along with being static_assert-able.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer> && !std::is_signed_v<Integer>, int> = 0>
	[[nodiscard]] constexpr bool signbit(Integer /* unused */) noexcept
	{
		// If the number is unsigned, then it can't be negative.
		return false;
	}

} // namespace ccm

// Clean up the global namespace
#ifdef CCMATH_HAS_CONSTEXPR_SIGNBIT
	#undef CCMATH_HAS_CONSTEXPR_SIGNBIT
#endif

#ifdef CCMATH_HAS_BUILTIN_BIT_CAST
	#undef CCMATH_HAS_BUILTIN_BIT_CAST
#endif

#ifdef CCMATH_HAS_CONSTEXPR_BUILTIN_SIGNBIT
	#undef CCMATH_HAS_CONSTEXPR_BUILTIN_SIGNBIT
#endif

#ifdef CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
	#undef CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
#endif
