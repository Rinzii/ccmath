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
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true> constexpr T nextup(T x) noexcept
	{
		return ccm::nextafter(x, std::numeric_limits<T>::infinity());
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true> constexpr double nextup(Integer x) noexcept
	{
		return ccm::nextup(static_cast<double>(x));
	}

	constexpr float nextupf(float x) noexcept
	{
		return ccm::nextup<float>(x);
	}

	constexpr long double nextupl(long double x) noexcept
	{
		return ccm::nextup<long double>(x);
	}
} // namespace ccm
