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

#include "ccmath/internal/math/generic/func/fmanip/impl/ilogb_impl.hpp"

#include <type_traits>

namespace ccm::gen
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr int ilogb_gen(T num) noexcept
	{
		if constexpr (std::is_same_v<T, float>) { return ccm::internal::impl::ilogb_impl(num); } // NOLINT(bugprone-branch-clone)
		else if constexpr (std::is_same_v<T, double>) { return ccm::internal::impl::ilogb_impl(num); }
		else
		{
			return ccm::internal::impl::ilogb_impl(static_cast<double>(num));
		}
	}
} // namespace ccm::gen
