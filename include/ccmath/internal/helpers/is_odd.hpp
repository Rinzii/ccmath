/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

namespace ccm::helpers
{
	template <typename T>
    [[nodiscard]]
    inline constexpr bool is_odd(T value) noexcept
    {
        return value % 2 != 0;
    }
}
