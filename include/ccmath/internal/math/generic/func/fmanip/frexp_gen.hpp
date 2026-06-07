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

#include "ccmath/internal/math/generic/func/fmanip/impl/frexp_impl.hpp"

#include <type_traits>

namespace ccm::gen
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T frexp_gen(T x, int & exp)
	{
		if constexpr (std::is_same_v<T, float>) { return ccm::internal::impl::frexp_impl(x, exp); }
		if constexpr (std::is_same_v<T, double>) { return ccm::internal::impl::frexp_impl(x, exp); }
		return static_cast<T>(ccm::internal::impl::frexp_impl(static_cast<double>(x), exp));
	}
} // namespace ccm::gen
