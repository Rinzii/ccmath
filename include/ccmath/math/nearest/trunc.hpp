/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/basic/abs.hpp"
#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/signbit.hpp"

namespace ccm
{
	/**
	 * @brief Returns the integral value nearest to x with the magnitude of the integral value always less than or equal to x.
	 * @tparam T The type of the input.
	 * @param num The value to truncate.
	 * @return Returns a truncated value.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T trunc(T num) noexcept
	{
		// If x is NaN then return Positive NaN or Negative NaN depending on the sign of x
		if (ccm::isnan(num))
		{
			if (ccm::signbit<T>(num)) { return -std::numeric_limits<T>::quiet_NaN(); }
			return std::numeric_limits<T>::quiet_NaN();
		}

		// If x == ±∞ then return x
		// If x == ±0 then return x
		if (ccm::isinf(num) || num == static_cast<T>(0.0)) { return num; }

		return static_cast<T>(static_cast<long long>(num));
	}

	/**
	 * @brief Returns the integral value nearest to x with the magnitude of the integral value always less than or equal to x.
	 * @tparam Integer The type of the input.
	 * @param num The value to truncate.
	 * @return Returns a truncated value.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double trunc(Integer num) noexcept
	{
		return static_cast<double>(num);
	}

	/**
	 * @brief Specialization for float that returns the integral value nearest to x with the magnitude of the integral value always less than or equal to x.
	 * @param num The float to truncate.
	 * @return Returns a truncated float.
	 */
	constexpr float truncf(float num) noexcept
	{
		return ccm::trunc<float>(num);
	}

	/**
	 * @brief Specialization for long double that returns the integral value nearest to x with the magnitude of the integral value always less than or equal to
	 * x.
	 * @param num The long double to truncate.
	 * @return Returns a truncated long double.
	 */
	constexpr long double truncl(long double num) noexcept
	{
		return ccm::trunc<long double>(num);
	}
} // namespace ccm
