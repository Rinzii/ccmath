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

#include "ccmath/internal/math/generic/builtins/trig/atan2.hpp"
#include "ccmath/math/trig/impl/inv_trig_impl.hpp"

#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T atan2(T y, T x)
	{
		if constexpr (ccm::builtin::has_constexpr_atan2<T>) { return ccm::builtin::atan2(y, x); }
		else if constexpr (std::is_same_v<T, float>) { return internal::impl::atan2_float(y, x); }
		else if constexpr (std::is_same_v<T, double>) { return internal::impl::atan2_double(y, x); }
		else { return static_cast<long double>(internal::impl::atan2_double(static_cast<double>(y), static_cast<double>(x))); }
	}

	constexpr float atan2f(float y, float x)
	{
		return ccm::atan2<float>(y, x);
	}

	constexpr long double atan2l(long double y, long double x)
	{
		return ccm::atan2<long double>(y, x);
	}
} // namespace ccm

/// @ingroup trig
