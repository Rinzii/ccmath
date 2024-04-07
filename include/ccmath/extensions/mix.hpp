/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <type_traits>

namespace ccm
{
    template <typename T>
    constexpr T mix(T x, T y, T a) noexcept
	{
        return x * (1 - a) + y * a;
    }

	template <typename TStart, typename TEnd, typename TAplha>
	constexpr std::common_type_t<TStart, TEnd, TAplha> mix(TStart x, TEnd y, TAplha a) noexcept
    {
        return x * (1 - a) + y * a;
    }
} // namespace ccm
