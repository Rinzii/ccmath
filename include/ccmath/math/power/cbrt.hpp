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

#include "ccmath/internal/math/generic/builtins/power/cbrt.hpp"
#include "ccmath/internal/math/runtime/func/power/cbrt_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/power/impl/cbrt_impl.hpp"

#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T cbrt(T num) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_cbrt<T>) { return ccm::builtin::cbrt(num); }
		else if (ccm::support::is_constant_evaluated())
		{
			if constexpr (std::is_same_v<T, float>) { return internal::impl::cbrt_impl(num); }
			else if constexpr (std::is_same_v<T, double>) { return internal::impl::cbrt_impl(num); }
			else { return static_cast<long double>(internal::impl::cbrt_impl(static_cast<double>(num))); }
		}
		else { return ccm::rt::cbrt_rt(num); }
	}

	constexpr float cbrtf(float num) noexcept
	{
		return ccm::cbrt(num);
	}

	constexpr long double cbrtl(long double num) noexcept
	{
		return ccm::cbrt(num);
	}
} // namespace ccm

/// @ingroup power
