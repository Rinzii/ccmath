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
    constexpr std::uint32_t CCMATH_VERSION_MAJOR = 0U;
    constexpr std::uint32_t CCMATH_VERSION_MINOR = 2U;
    constexpr std::uint32_t CCMATH_VERSION_PATCH = 0U;
    constexpr std::uint32_t CCMATH_VERSION = ((0U << 22U) | (2U << 12U) | (0U));
} // namespace ccm::config
