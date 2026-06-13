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

#include "ccmath/math/expo/impl/exp_double_impl.hpp"
#include "ccmath/math/expo/impl/exp_float_impl.hpp"

namespace ccm::internal
{
	constexpr float expm1_float(float num) noexcept
	{ return impl::expm1_float_impl(num); }

	constexpr double expm1_double(double num) noexcept
	{ return impl::expm1_double_impl(num); }
} // namespace ccm::internal
