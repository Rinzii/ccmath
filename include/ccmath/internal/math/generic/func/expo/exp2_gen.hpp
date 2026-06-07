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

#include "ccmath/math/expo/impl/exp2_double_impl.hpp"
#include "ccmath/math/expo/impl/exp2_float_impl.hpp"

#include <type_traits>

namespace ccm::gen
{
	// TODO(IanP): Wire internal poly_approx_* kernels here instead of delegating to exp2_*_impl.
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T exp2_gen(T num) noexcept
	{
		if constexpr (std::is_same_v<T, float>) { return ccm::internal::exp2_float(num); }
		if constexpr (std::is_same_v<T, double>) { return ccm::internal::exp2_double(num); }
		if constexpr (std::is_same_v<T, long double>) { return static_cast<long double>(ccm::internal::exp2_double(static_cast<double>(num))); }
		return static_cast<T>(ccm::internal::exp2_double(static_cast<double>(num)));
	}
} // namespace ccm::gen
