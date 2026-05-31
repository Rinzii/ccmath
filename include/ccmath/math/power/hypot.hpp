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

#include "ccmath/internal/math/generic/builtins/power/hypot.hpp"
#include "ccmath/math/power/impl/hypot_impl.hpp"

#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T hypot(T x, T y) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_hypot<T>) { return ccm::builtin::hypot(x, y); }
		else if constexpr (std::is_same_v<T, float>) { return internal::impl::hypot_impl(x, y); }
		else if constexpr (std::is_same_v<T, double>) { return internal::impl::hypot_impl(x, y); }
		else { return static_cast<long double>(internal::impl::hypot_impl(static_cast<double>(x), static_cast<double>(y))); }
	}

	constexpr float hypotf(float x, float y) noexcept
	{
		return ccm::hypot(x, y);
	}

	constexpr long double hypotl(long double x, long double y) noexcept
	{
		return ccm::hypot(x, y);
	}
} // namespace ccm

/// @ingroup power
