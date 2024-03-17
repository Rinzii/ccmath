/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/isnan.hpp"

#include <limits>
#include <type_traits>

namespace ccm
{
	/**
     * @brief Computes the largest integer value not greater than num.
     * @tparam T The type of the number.
     * @param num A floating-point or integer value.
     * @return If no errors occur, the largest integer value not greater than num, that is ⌊num⌋, is returned.
	 */
	template <typename T>
	inline constexpr T floor(T num) noexcept
    {
		if constexpr (std::is_floating_point_v<T>)
		{
			// If num is NaN, NaN is returned.
			if (ccm::isnan(num))
            {
                return std::numeric_limits<T>::quiet_NaN();
            }

			// If num is ±∞ or ±0, num is returned, unmodified.
			if (ccm::isinf(num) || num == static_cast<T>(0))
            {
                return num;
            }
        }

		// Compute the largest integer value not greater than num.
		return static_cast<T>(static_cast<long long int>(num));
    }

	/**
     * @brief Computes the largest integer value not greater than num.
     * @param num A integer value.
     * @return If no errors occur, the largest integer value not greater than num, that is ⌊num⌋, is returned.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	inline constexpr double floor(Integer num) noexcept
	{
		return static_cast<double>(num); // All integers already have a floor value. Just cast to double and return.
	}

	/**
     * @brief Computes the largest integer value not greater than num.
     * @param num A floating-point value.
     * @return If no errors occur, the largest integer value not greater than num, that is ⌊num⌋, is returned.
	 */
	inline constexpr float floorf(float num) noexcept
    {
        return floor<float>(num);
    }

	/**
     * @brief Computes the largest integer value not greater than num.
     * @param num A floating-point value.
     * @return If no errors occur, the largest integer value not greater than num, that is ⌊num⌋, is returned.
	 */
	inline constexpr double floorl(double num) noexcept
    {
        return floor<double>(num);
    }
} // namespace ccm

/// @ingroup nearest
