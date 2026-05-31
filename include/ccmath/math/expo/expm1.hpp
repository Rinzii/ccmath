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
#include "ccmath/internal/math/runtime/func/expo/expm1_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/expo/impl/expm1_impl.hpp"

#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T expm1(T num)
	{
		if constexpr (ccm::builtin::has_constexpr_expm1<T>) { return ccm::builtin::expm1(num); }
		else
		{
			if (!ccm::support::is_constant_evaluated()) { return ccm::rt::expm1_rt(num); }

			if constexpr (std::is_same_v<T, float>) { return internal::expm1_float(num); }
			if constexpr (std::is_same_v<T, double>) { return internal::expm1_double(num); }
			if constexpr (std::is_same_v<T, long double>) { return static_cast<long double>(internal::expm1_double(static_cast<double>(num))); }
			return static_cast<T>(internal::expm1_double(static_cast<double>(num)));
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
