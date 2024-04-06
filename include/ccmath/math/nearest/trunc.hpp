/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/basic/abs.hpp"
#include "ccmath/math/compare/signbit.hpp"

namespace ccm
{
	/**
	 * @brief Returns the integral value nearest to x with the magnitude of the integral value always less than or equal to x.
	 * @tparam T The type of the input.
	 * @param x The value to truncate.
	 * @return Returns a truncated value.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, int> = 0>
	constexpr T trunc(T x) noexcept
	{
		// If x is NaN then return Positive NaN or Negative NaN depending on the sign of x
		if (ccm::isnan(x))
		{
			if (ccm::signbit<T>(x)) { return -std::numeric_limits<T>::quiet_NaN(); }
			return std::numeric_limits<T>::quiet_NaN();
		}

		// If x == ±∞ then return x
		if (x == std::numeric_limits<T>::infinity() || x == -std::numeric_limits<T>::infinity()) { return x; }

		// If x == ±0 then return x
		if (x == static_cast<T>(0.0)) { return x; }

		return static_cast<T>(static_cast<long long>(x));
	}

	/**
	 * @brief Returns the integral value nearest to x with the magnitude of the integral value always less than or equal to x.
	 * @tparam Integer The type of the input.
	 * @param x The value to truncate.
	 * @return Returns a truncated value.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, int> = 0>
	constexpr double trunc(Integer x) noexcept
	{
		return static_cast<double>(x);
	}

	/**
	 * @brief Specialization for float that returns the integral value nearest to x with the magnitude of the integral value always less than or equal to x.
	 * @param x The float to truncate.
	 * @return Returns a truncated float.
	 */
	constexpr float truncf(float x) noexcept
	{
		return trunc<float>(x);
	}

	/**
	 * @brief Specialization for long double that returns the integral value nearest to x with the magnitude of the integral value always less than or equal to
	 * x.
	 * @param x The long double to truncate.
	 * @return Returns a truncated long double.
	 */
	constexpr long double truncl(long double x) noexcept
	{
		return trunc<long double>(x);
	}
} // namespace ccm
