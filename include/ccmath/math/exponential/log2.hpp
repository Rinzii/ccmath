/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/internal/config/compiler.hpp"
#include "ccmath/math/compare/signbit.hpp"
#include "ccmath/math/exponential/impl/log2_double_impl.hpp"
#include "ccmath/math/exponential/impl/log2_float_impl.hpp"

#include <limits>
#include <type_traits>

namespace ccm
{
	/**
	 * @brief Returns the base 2 logarithm of a number.
	 * @tparam T The type of the number.
	 * @param num The number to calculate the base 2 logarithm of.
	 * @return The base 2 logarithm of the number.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T log2(T num) noexcept
	{
#if defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__)
		if constexpr (std::is_same_v<T, float>) { return __builtin_log2f(num); }
		if constexpr (std::is_same_v<T, double>) { return __builtin_log2(num); }
		if constexpr (std::is_same_v<T, long double>) { return __builtin_log2l(num); }
		return static_cast<T>(__builtin_log2l(num));
#else
		// If the argument is ±0, -∞ is returned
		if (num == static_cast<T>(0)) { return -std::numeric_limits<T>::infinity(); }

		// If the argument is 1, +0 is returned.
		if (num == static_cast<T>(1)) { return 0; }

		// If the argument is NaN, NaN is returned.
		if (ccm::isnan(num) || num == std::numeric_limits<T>::infinity())
		{
			return num;
		}

		// If the argument is negative, -NaN is returned
		#ifdef CCMATH_COMPILER_APPLE_CLANG // Apple clang returns +qNaN
		if (ccm::signbit(num)) { return std::numeric_limits<T>::quiet_NaN(); }
		#else // All other major compilers return -qNaN
		if (ccm::signbit(num)) { return -std::numeric_limits<T>::quiet_NaN(); }
		#endif

		// We cannot handle long double at this time due to problems
		// with long double being platform-dependent with its bit size.
		if constexpr (std::is_same_v<T, float>) { return internal::log2_float(num); }
		if constexpr (std::is_same_v<T, double>) { return internal::log2_double(num); }
		if constexpr (std::is_same_v<T, long double>) { return static_cast<long double>(internal::log2_double(static_cast<double>(num))); }
		return static_cast<T>(internal::log2_double(static_cast<double>(num)));
#endif
	}

	/**
	 * @brief Returns the base 2 logarithm of a number.
	 * @tparam Integer The type of the integer.
	 * @param num The number to calculate the base 2 logarithm of.
	 * @return The base 2 logarithm of the number as a double.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double log2(Integer num) noexcept
	{
		return ccm::log2<double>(static_cast<double>(num));
	}

	/**
	 * @brief Returns the base 2 logarithm of a number.
	 * @param num The number to calculate the base 2 logarithm of.
	 * @return The base 2 logarithm of the number as a float.
	 */
	constexpr float log2f(float num)
	{
		return ccm::log2<float>(num);
	}

	/**
	 * @brief Returns the base 2 logarithm of a number.
	 * @param num The number to calculate the base 2 logarithm of.
	 * @return The base 2 logarithm of the number as a double.
	 */
	constexpr long double log2l(long double num)
	{
		return ccm::log2<long double>(num);
	}
} // namespace ccm
