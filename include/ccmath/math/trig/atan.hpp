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

#include "ccmath/internal/math/generic/builtins/trig/atan.hpp"
#include "ccmath/internal/math/runtime/func/trig/atan_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/trig/impl/inv_trig_impl.hpp"

#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T atan(T num)
	{
		if constexpr (ccm::builtin::has_constexpr_atan<T>) { return ccm::builtin::atan(num); }
		else if (ccm::support::is_constant_evaluated())
		{
			if constexpr (std::is_same_v<T, float>) { return internal::impl::atan_float(num); }
			else if constexpr (std::is_same_v<T, double>) { return internal::impl::atan_double(num); }
			else { return static_cast<long double>(internal::impl::atan_double(static_cast<double>(num))); }
		}
		else { return ccm::rt::atan_rt(num); }
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double atan(Integer num)
	{
		return ccm::atan<double>(static_cast<double>(num));
	}

	constexpr float atanf(float num)
	{
		return ccm::atan<float>(num);
	}

	constexpr long double atanl(long double num)
	{
		return ccm::atan<long double>(num);
	}
} // namespace ccm

/// @ingroup trig
