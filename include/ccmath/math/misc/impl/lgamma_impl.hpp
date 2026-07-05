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

#include "ccmath/math/misc/impl/lgamma_double_impl.hpp"

namespace ccm::internal
{
	constexpr float lgamma_float(float x) noexcept
	{
		return impl::lgamma_float_impl(x);
	}

	constexpr double lgamma_double(double x) noexcept
	{
		return impl::lgamma_double_impl(x);
	}
} // namespace ccm::internal
