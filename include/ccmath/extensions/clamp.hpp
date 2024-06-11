/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/basic/min.hpp"
#include "ccmath/math/basic/max.hpp"
#include <type_traits>

namespace ccm::ext
{
	/**
     * @brief Clamps a value between a minimum and maximum value.
     * @tparam T Type of the input and output.
     * @param x Value to clamp.
     * @param lo Minimum value.
     * @param hi Maximum value.
     * @return The clamped value.
     */
    template<typename T>
    constexpr T clamp(T v, T lo = T{0}, T hi = T{1})
    {
        return ccm::min(ccm::max(v, lo), hi);
    }

	/**
     * @brief Clamps a value between a minimum and maximum value.
     * @tparam TVal Type of the input value.
     * @tparam TLow Type of the lower bound.
     * @tparam THigh Type of the upper bound.
     * @param v Value to clamp.
     * @param lo Minimum value.
     * @param hi Maximum value.
     * @return The clamped value.
     */
	template<typename TVal, typename TLow, typename THigh>
    constexpr std::common_type_t<TVal, TLow, THigh> clamp(TVal v, TLow lo = TLow{0}, THigh hi = THigh{1})
    {
        return ccm::min(ccm::max(v, lo), hi);
    }
} // namespace ccm::ext
