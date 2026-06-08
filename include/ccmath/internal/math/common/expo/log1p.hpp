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

#include "ccmath/internal/math/generic/builtins/expo/log1p.hpp"
#include "ccmath/internal/math/generic/func/expo/log1p_gen.hpp"

#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T log1p(T num)
	{
		if constexpr (ccm::builtin::has_constexpr_log1p<T>) { return ccm::builtin::log1p(num); }
		else
		{
			return gen::log1p_gen(num);
		}
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double log1p(Integer num)
	{ return ccm::log1p<double>(static_cast<double>(num)); }

	constexpr float log1pf(float num)
	{ return ccm::log1p<float>(num); }

	constexpr long double log1pl(long double num)
	{ return ccm::log1p<long double>(num); }
} // namespace ccm
