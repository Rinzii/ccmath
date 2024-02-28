/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/detail/compare/isnan.hpp"

// If we have C++23, we can use std::signbit as it is constexpr
#if (defined(__cpp_lib_constexpr_cmath) && __cpp_lib_constexpr_cmath >= 202202L)
	#include <cmath>
	#ifndef CCMATH_HAS_CONSTEXPR_SIGNBIT
		#define CCMATH_HAS_CONSTEXPR_SIGNBIT
	#endif
#endif

// We only implement this for MSVC as that is the only manner to get constexpr signbit that is also static_assert-able
#ifndef CCMATH_HAS_BUILTIN_BIT_CAST
	#if (defined(_MSC_VER) && _MSC_VER >= 1927)
		#define CCMATH_HAS_BUILTIN_BIT_CAST
		#include <limits>  // for std::numeric_limits
		#include <cstdint> // for std::uint64_t
	#endif
#endif

// Some compilers have __builtin_signbit which is constexpr for some compilers
#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_SIGNBIT
	#if !defined(CCMATH_HAS_CONSTEXPR_SIGNBIT)
		// GCC 6.1 has constexpr __builtin_signbit that DOES allow static_assert
		#if defined(__GNUC__) && (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)
			#define CCMATH_HAS_CONSTEXPR_BUILTIN_SIGNBIT
		#endif
	#endif
#endif

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
	#if !defined(CCMATH_HAS_CONSTEXPR_SIGNBIT)
		// GCC 6.1 has constexpr __builtin_copysign that DOES allow static_assert
		#if defined(__GNUC__) && (__GNUC__ >= 6 && __GNUC_MINOR__ >= 1)
			#define CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
		#endif

		// Clang 5.0.0 has constexpr __builtin_copysign that DOES allow static_assert
		#if defined(__clang__) && (__clang_major__ >= 5)
			#define CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN
		#endif
	#endif
#endif

#if defined(_MSVC_VER) && !defined(CCMATH_HAS_CONSTEXPR_BUILTIN_BIT_CAST) && !defined(CCMATH_HAS_CONSTEXPR_SIGNBIT) &&                                         \
	!defined(CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN)
	#define CCMATH_MSVC_DOES_NOT_HAVE_ASSERTABLE_CONSTEXPR_SIGNBIT
	#include "float.h" // for _fpclass and _FPCLASS_NZ
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
	 * the same values as std::signbit along with being static_assert-able. The only exception is MSVC 19.26 and earlier
	 * and unknown compilers that support none of possible implementations.
	 *
	 * @note ccm::signbit by default will use std::signbit if using C++23 or later.
	 *
	 * @warning ccm::signbit will not work with static_assert on MSVC 19.26 and earlier. This is due to the fact that
	 * MSVC does not provide a constexpr signbit until 19.27. This is a limitation of MSVC and not ccmath.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
	[[nodiscard]] inline constexpr bool signbit(T x) noexcept
	{
#if defined(CCMATH_HAS_CONSTEXPR_SIGNBIT)
		return std::signbit(x);
#elif defined(CCMATH_HAS_CONSTEXPR_BUILTIN_SIGNBIT)
		return __builtin_signbit(x);
#elif defined(CCMATH_HAS_CONSTEXPR_BUILTIN_COPYSIGN)
		// use __builtin_copysign to check for the sign of zero

		if (x == 0 || ccm::isnan(x))
		{
			// If constexpr only works with gcc 7.1+. Without if constexpr we work till GCC 5.1+
			// This works with clang 5.0.0 no problem even with if constexpr
			// TODO: Add a compatibility layer for if constexpr to allow us to continue using gcc 5.1+
			if constexpr (std::is_same_v<T, float>) { return __builtin_copysignf(1.0f, x) < 0; }
			else if constexpr (std::is_same_v<T, double>) { return __builtin_copysign(1.0, x) < 0; }
			else if constexpr (std::is_same_v<T, long double>) { return __builtin_copysignl(1.0l, x) < 0; }

			return false;
		}

		return x < static_cast<T>(0);
#elif defined(CCMATH_HAS_BUILTIN_BIT_CAST)
		// Check for the sign of +0.0 and -0.0 with __builtin_bit_cast
		if (x == 0)
		{
			constexpr auto signbit_mask = static_cast<std::uint64_t>(1) << (std::numeric_limits<std::uint64_t>::digits - 1);
			const auto bits				= __builtin_bit_cast(std::uint64_t, x);
			return (bits & signbit_mask) != 0;
		}

		// This does not work for -0.0, but there is no way to check for -0.0 without doing extreme UB.
		return x < static_cast<T>(0);
#elif defined(CCMATH_MSVC_DOES_NOT_HAVE_ASSERTABLE_CONSTEXPR_SIGNBIT)
		// If we don't have access to MSBC 19.27 or later, we can use _fpclass and _FPCLASS_NZ to
		// check for the sign of zero. This is is constexpr, but it is not static_assert-able.
		return ((x == T(0)) ? (_fpclass(x) == _FPCLASS_NZ) : (x < T(0))); // This won't work in static assertions
#else
		static_assert(false, "signbit is not implemented for this compiler");
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
	template <typename T, std::enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value, int> = 0>
	[[nodiscard]] inline constexpr bool signbit(T x) noexcept
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
	template <typename T, std::enable_if_t<std::is_integral<T>::value && !std::is_signed<T>::value, int> = 0>
	[[nodiscard]] inline constexpr bool signbit(T /* unused */) noexcept
	{
		// If the number is unsigned then it can't be negative.
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

#ifdef CCMATH_MSVC_DOES_NOT_HAVE_ASSERTABLE_CONSTEXPR_SIGNBIT
    #undef CCMATH_MSVC_DOES_NOT_HAVE_ASSERTABLE_CONSTEXPR_SIGNBIT
#endif
