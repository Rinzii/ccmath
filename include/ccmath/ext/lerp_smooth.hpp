/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * This library is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#pragma once

#include "ccmath/math/expo/exp2.hpp"

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
	    // ReSharper disable once CppRedundantParentheses
	    return b + ((a - b) * ccm::exp2<T>(-t / h));
    }
} // namespace ccm::ext
