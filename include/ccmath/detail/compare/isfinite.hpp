/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/detail/compare/isnan.hpp"
#include "ccmath/detail/compare/isinf.hpp"

namespace ccm
{
	/**
	 * @brief Checks if the given number has a finite value.
	 * @tparam T The type of the number.
	 * @param num A floating-point or integer value
	 * @return true if the number has a finite value, false otherwise.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	inline constexpr bool isfinite(T num)
	{
		return (!ccm::isnan(num)) && (!ccm::isinf(num));
	}

	/**
     * @brief Checks if the given number has a finite value.
     * @tparam Integer The type of the integer.
     * @param num A integer value that will be converted to a double.
     * @return true if the number has a finite value, false otherwise.
     */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	inline constexpr bool isfinite(Integer num)
    {
        return isfinite<double>(static_cast<double>(num));
    }

} // namespace ccm
