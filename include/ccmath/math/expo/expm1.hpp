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

#include "ccmath/internal/math/generic/builtins/expo/expm1.hpp"

#include <type_traits>

// TODO: Implement this.

namespace ccm
{
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T expm1([[maybe_unused]] T num)
	{
		if constexpr (ccm::builtin::has_constexpr_expm1<T>) { return ccm::builtin::expm1(num); }
		else
		{
			if constexpr (std::is_same_v<T, float>) { return 0; }
			if constexpr (std::is_same_v<T, double>) { return 0; }
			if constexpr (std::is_same_v<T, long double>) { return 0; }
			return 0;
		}
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double expm1(Integer num)
	{
		return ccm::expm1<double>(static_cast<double>(num));
	}

	constexpr float expm1f(float num)
	{
		return ccm::expm1<float>(num);
	}

	constexpr long double expm1l(long double num)
	{
		return ccm::expm1<long double>(num);
	}
} // namespace ccm
