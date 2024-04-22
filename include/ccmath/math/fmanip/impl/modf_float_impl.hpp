/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/bits.hpp"

namespace ccm::internal::impl
{
	constexpr double modf_float_impl(float x, float* iptr) noexcept
	{
		__float128 q = x;

		return 0;
	}

} // namespace ccm::internal::impl