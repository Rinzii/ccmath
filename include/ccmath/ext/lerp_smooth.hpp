/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/exponential/exp2.hpp"

namespace ccm::ext
{
	/**
	 * @brief Frame rate independent linear interpolation smoothing.
	 * @tparam T Type of the input and output.
	 * @param a Current value.
	 * @param b Target value.
	 * @param t Delta time, in seconds.
	 * @param h Half-life, time until halfway, in seconds.
	 * @return The smoothed value.
	 *
	 * @warning Currently waiting on ccm::exp2 to be implemented. Till then this will NOT work.
	 */
	template<typename T>
    constexpr T lerp_smooth(T a, T b, T t, T h)
    {
        return b + (a - b) * ccm::exp2<T>(-t / h);
    }
} // namespace ccm::ext
