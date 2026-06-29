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

#include "ccmath/math/expo/impl/log_double_impl.hpp"
#include "ccmath/math/expo/impl/log_float_impl.hpp"
#include "ccmath/math/numbers.hpp"

namespace ccm::internal::impl
{
	constexpr float log10_float_impl(float x)
	{
		return log_float_impl(x) * static_cast<float>(numbers::log10e_v<float>);
	}

	constexpr double log10_double_impl(double x)
	{
		return log_double_impl(x) * numbers::log10e_v<double>;
	}
} // namespace ccm::internal::impl

namespace ccm::internal
{
	constexpr float log10_float(float num) noexcept
	{
		return impl::log10_float_impl(num);
	}

	constexpr double log10_double(double num) noexcept
	{
		return impl::log10_double_impl(num);
	}
} // namespace ccm::internal
