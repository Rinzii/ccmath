/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/detail/compare/isnan.hpp"
#include "ccmath/detail/compare/signbit.hpp"
#include "ccmath/detail/exponential/details/log2_double_impl.hpp"
#include "ccmath/detail/exponential/details/log2_float_impl.hpp"

#include <limits>
#include <type_traits>

namespace ccm
{
	/**
	 * @brief Returns the base 2 logarithm of a number.
	 *
	 * @param num The number to calculate the base 2 logarithm of.
	 * @return The base 2 logarithm of the number.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, int> = 0>
	inline constexpr T log2(T num) noexcept
	{
		// If the argument is ±0, -∞ is returned
		if (num == static_cast<T>(0)) { return -std::numeric_limits<T>::infinity(); }

		// If the argument is 1, +0 is returned.
		if (num == static_cast<T>(1)) { return 0; }

		// If the argument is negative, -NaN is returned
		if (ccm::signbit(num)) { return -std::numeric_limits<T>::quiet_NaN(); }

		// If the argument is +∞, +∞ is returned.
		if (num == std::numeric_limits<T>::infinity()) { return std::numeric_limits<T>::infinity(); }

		// If the argument is NaN, NaN is returned.
		if (ccm::isnan(num))
		{
			if (ccm::signbit(num)) { return -std::numeric_limits<T>::quiet_NaN(); }
			else { return std::numeric_limits<T>::quiet_NaN(); }
		}

		if constexpr (std::is_same_v<T, float>) { return ccm::internal::log2_float(num); }
		else { return ccm::internal::log2_double(num); }
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, int> = 0>
	inline constexpr double log2(Integer num) noexcept
	{
		return ccm::log2(static_cast<double>(num));
	}

	inline constexpr float log2f(float num)
	{
		return ccm::log2(num);
	}

	inline constexpr long double log2l(long double num)
	{
		return ccm::log2(num);
	}
} // namespace ccm
