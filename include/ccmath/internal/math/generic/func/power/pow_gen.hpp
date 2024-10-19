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

#include "ccmath/internal/math/generic/func/power/pow_impl.hpp"
#include "ccmath/internal/math/generic/func/power/powf_impl.hpp"

#include <type_traits>

namespace ccm::gen
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T pow_gen(T base, T exp) noexcept
	{
		// TODO: Maybe add more specific optimizations for integers?
		// Currently we only support float and double for all rounding modes
		if constexpr (std::is_same_v<T, float>) { return impl::powf_impl(base, exp); }
		else if constexpr (std::is_same_v<T, double>) { return impl::pow_impl(base, exp); }
		else { return static_cast<T>(impl::pow_impl(static_cast<double>(base), static_cast<double>(exp))); }
	}

} // namespace ccm::gen
