/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/detail/basic/abs.hpp"
#include "ccmath/detail/nearest/floor.hpp"

// TODO: Waiting on floor

namespace ccm::helpers
{
	/**
	 * @brief Finds the whole number part of a floating-point number.
	 * @tparam T The type of the number.
	 * @param val The floating-point number to find the whole number part of.
	 * @return The whole number part of the floating-point number.
	 */
	template <typename T>
	inline constexpr long long int find_num_whole_part(const T val) noexcept
	{
		return static_cast<long long int>(ccm::floor(val));
	}

	template <typename T>
	inline constexpr T find_num_fract_part(const T val) noexcept
	{
		return val - static_cast<T>(find_num_whole_part(val));
	}

} // namespace ccm::helpers
