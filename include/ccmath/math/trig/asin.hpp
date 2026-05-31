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

#include "ccmath/internal/math/generic/builtins/trig/asin.hpp"
#include "ccmath/math/trig/impl/inv_trig_impl.hpp"

#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T asin(T num)
	{
		if constexpr (ccm::builtin::has_constexpr_asin<T>) { return ccm::builtin::asin(num); }
		else if constexpr (std::is_same_v<T, float>) { return internal::impl::asin_float(num); }
		else if constexpr (std::is_same_v<T, double>) { return internal::impl::asin_double(num); }
		else { return static_cast<long double>(internal::impl::asin_double(static_cast<double>(num))); }
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double asin(Integer num)
	{
		return ccm::asin<double>(static_cast<double>(num));
	}

	constexpr float asinf(float num)
	{
		return ccm::asin<float>(num);
	}

	constexpr long double asinl(long double num)
	{
		return ccm::asin<long double>(num);
	}
} // namespace ccm

/// @ingroup trig
