/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <cstdint>

namespace ccm::config
{
    constexpr std::uint32_t CCMATH_VERSION_MAJOR = 0;
    constexpr std::uint32_t CCMATH_VERSION_MINOR = 2;
    constexpr std::uint32_t CCMATH_VERSION_PATCH = 0;
    constexpr std::uint32_t CCMATH_VERSION = (
    (static_cast<std::uint32_t>(0) << 29U) |
    (static_cast<std::uint32_t>(0) << 22U) |
    (static_cast<std::uint32_t>(2) << 12U) |
    static_cast<std::uint32_t>(0));
}
