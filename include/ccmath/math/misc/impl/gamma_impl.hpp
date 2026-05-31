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

#include "ccmath/math/misc/impl/gamma_double_impl.hpp"
#include "ccmath/math/misc/impl/gamma_float_impl.hpp"

namespace ccm::internal
{
	constexpr float gamma_float(float x) noexcept
	{
		return impl::gamma_float_impl(x);
	}

	constexpr double gamma_double(double x) noexcept
	{
		return impl::gamma_double_impl(x);
	}
} // namespace ccm::internal
