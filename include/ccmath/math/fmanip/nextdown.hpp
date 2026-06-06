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

#include "ccmath/math/fmanip/nextafter.hpp"

#include <limits>
#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T nextdown(T x) noexcept
	{
		return ccm::nextafter(x, -std::numeric_limits<T>::infinity());
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double nextdown(Integer x) noexcept
	{
		return ccm::nextdown(static_cast<double>(x));
	}

	constexpr float nextdownf(float x) noexcept
	{
		return ccm::nextdown<float>(x);
	}

	constexpr long double nextdownl(long double x) noexcept
	{
		return ccm::nextdown<long double>(x);
	}
} // namespace ccm
