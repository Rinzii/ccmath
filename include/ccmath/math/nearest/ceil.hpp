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
     * @brief Computes the smallest integer value no lesser than num.
     * @tparam T The type of the number.
     * @param num A floating-point or integer value.
     * @return If no errors occur, the smallest integer value no lesser than num, that is ⌈num⌉, is returned.
     */
    template <typename T>
    constexpr T ceil(T num) noexcept 
    {
        if constexpr (std::is_floating_point_v<T>)
		{
			// If num is NaN, NaN is returned.
			if (ccm::isnan(num)) { return std::numeric_limits<T>::quiet_NaN(); }

			// If num is ±∞ or ±0, num is returned, unmodified.
			if (ccm::isinf(num) || num == static_cast<T>(0)) { return num; }

            // Compute the largest integer value not greater than num.
            T num_floored = static_cast<T>(static_cast<long long int>(num));

            // if num_floored is not equal to num, num_floored incremented is returned
            if (num_floored != num) { return num_floored + 1; } 
		}

        // if num is an integer, num is returned, unmodified
        if constexpr (std::is_integral_v<T>) { return num; }
    }

    /**
     * @brief Computes the smallest integer value no lesser than num.
     * @param num An integer value.
     * @return If no errors occur, the smallest integer value no lesser than num, that is ⌈num⌉, is returned. 
     */
    template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
    constexpr double ceil(Integer num) noexcept 
    {
        return static_cast<double>(num); // All integers already have a ceil value. Just cast to double and return.
    }

    /**
     * @brief Computes the smallest integer value no lesser than num.
     * @param num A floating-point value.
     * @return If no errors occur, the smallest integer value no lesser than num, that is ⌈num⌉, is returned. 
     */
    constexpr float ceilf(float num) noexcept
    {
        return ceil<float>(num);
    }

    /**
     * @brief Computes the smallest integer value no lesser than num.
     * @param num A floating-point value.
     * @return If no errors occur, the smallest integer value no lesser than num, that is ⌈num⌉, is returned. 
     */
    constexpr double ceill(double num) noexcept
    {
        return ceil<double>(num);
    }
} // namespace ccm

/// @ingroup nearest
