/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/numbers.hpp"
#include <type_traits>

namespace ccm::ext
{
	/**
	 * @brief Converts an angle in degrees to radians.
	 * @tparam T Floating-point type.
	 * @param degrees Angle in degrees.
	 * @return The angle in radians.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T radians(T degrees) noexcept
	{
		return (ccm::numbers::pi_v<T> * degrees) / static_cast<T>(180);
	}
} // namespace ccm::ext
