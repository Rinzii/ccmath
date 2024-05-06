/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/extensions/clamp.hpp"
#include <type_traits>

namespace ccm::ext
{
	/**
	 * @brief Normalize a value to a range.
	 * @tparam T Type of the input and output.
	 * @param value The value to normalize.
	 * @param min The minimum value of the range.
	 * @param max The maximum value of the range.
	 * @return The normalized value.
	 */
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T normalize(T value, T min = T(0), T max = T(1))
	{
		return ccm::ext::clamp((value - min) / (max - min), static_cast<T>(0), static_cast<T>(1));
	}
} // namespace ccm::ext