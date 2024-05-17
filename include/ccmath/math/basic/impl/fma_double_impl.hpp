/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/math/compare/signbit.hpp"

#include <limits>
#include <type_traits>

namespace ccm::internal::impl
{

	constexpr float fma_double(float x, float y, float z) noexcept
    {
    }

} // namespace ccm::internal::impl