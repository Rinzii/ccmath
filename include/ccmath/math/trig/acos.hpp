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

#include "ccmath/internal/math/generic/builtins/trig/acos.hpp"
#include "ccmath/math/trig/impl/inv_trig_impl.hpp"

#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T acos(T num)
	{
		if constexpr (ccm::builtin::has_constexpr_acos<T>) { return ccm::builtin::acos(num); }
		else if constexpr (std::is_same_v<T, float>) { return internal::impl::acos_float(num); }
		else if constexpr (std::is_same_v<T, double>) { return internal::impl::acos_double(num); }
		else { return static_cast<long double>(internal::impl::acos_double(static_cast<double>(num))); }
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double acos(Integer num)
	{
		return ccm::acos<double>(static_cast<double>(num));
	}

	constexpr float acosf(float num)
	{
		return ccm::acos<float>(num);
	}

	constexpr long double acosl(long double num)
	{
		return ccm::acos<long double>(num);
	}
} // namespace ccm

/// @ingroup trig
