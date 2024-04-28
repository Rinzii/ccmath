/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <type_traits>

namespace ccm::ext
{
	/**
	 * @brief Calculates a fast, approximate, reciprocal.
	 * @tparam T Type of the input and output.
	 * @param x Value to get the reciprocal of.
	 * @return The reciprocal of the input.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T rcp(T x) noexcept
	{
		return static_cast<T>(1) / x;
	}
} // namespace ccm::ext