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

#include <type_traits>

namespace ccm::gen
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true> constexpr T log_gen(T num) noexcept
	{
		if constexpr (std::is_same_v<T, float>)
		{
			return ccm::internal::log_float(num);
		} else if constexpr (std::is_same_v<T, double>)
		{
			return ccm::internal::log_double(num);
		} else
		{
			return static_cast<T>(ccm::internal::log_double(static_cast<double>(num)));
		}
	}
} // namespace ccm::gen
