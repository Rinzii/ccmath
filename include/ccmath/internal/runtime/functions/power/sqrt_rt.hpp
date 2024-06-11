/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/math/internal_sqrt.hpp"
//#include "ccmath/internal/runtime/simd/

template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
T sqrt_rt(T num)
{

}