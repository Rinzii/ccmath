/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#pragma once

#include "ccmath/internal/support/bits.hpp"

namespace ccm::internal::impl
{
	constexpr double modf_float_impl(float x, float * iptr) noexcept
	{
		//__float128 q = x;

		return 0;
	}

} // namespace ccm::internal::impl
