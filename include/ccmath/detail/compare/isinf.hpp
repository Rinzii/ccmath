/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <limits>
#include <type_traits>

namespace ccm
{
	/**
	 * @brief
	 * @tparam Real
	 * @param x
	 * @return
	 */
	template <typename Real, typename = std::enable_if_t<std::is_floating_point_v<Real>>>
    inline constexpr bool isinf(Real x) noexcept
    {
		if constexpr (std::numeric_limits<Real>::is_signed)
		{
			return x == -std::numeric_limits<Real>::infinity() || x == std::numeric_limits<Real>::infinity();
		}
		else
		{
			return x == std::numeric_limits<Real>::infinity();
		}
    }
} // namespace ccm
