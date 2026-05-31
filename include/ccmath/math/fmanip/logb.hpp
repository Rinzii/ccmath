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

#include "ccmath/internal/math/generic/builtins/fmanip/logb.hpp"
#include "ccmath/internal/math/runtime/func/fmanip/logb_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/fmanip/impl/logb_impl.hpp"

#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T logb(T num) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_logb<T>) { return ccm::builtin::logb(num); }
		else if (ccm::support::is_constant_evaluated())
		{
			if constexpr (std::is_same_v<T, float>) { return internal::impl::logb_impl(num); }
			else if constexpr (std::is_same_v<T, double>) { return internal::impl::logb_impl(num); }
			else { return static_cast<long double>(internal::impl::logb_impl(static_cast<double>(num))); }
		}
		else { return ccm::rt::logb_rt(num); }
	}

	constexpr float logbf(float num) noexcept
	{
		return ccm::logb(num);
	}

	constexpr long double logbl(long double num) noexcept
	{
		return ccm::logb(num);
	}
} // namespace ccm
