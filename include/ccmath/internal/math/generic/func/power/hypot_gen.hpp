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

#include "ccmath/internal/math/generic/func/power/impl/hypot_impl.hpp"

#include <type_traits>

namespace ccm::gen
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T hypot_gen(T x, T y) noexcept
	{
		if constexpr (std::is_same_v<T, float>) { return ccm::internal::impl::hypot_impl(x, y); }
		else if constexpr (std::is_same_v<T, double>) { return ccm::internal::impl::hypot_impl(x, y); }
		else
		{
			return static_cast<T>(ccm::internal::impl::hypot_impl(static_cast<double>(x), static_cast<double>(y)));
		}
	}
} // namespace ccm::gen
