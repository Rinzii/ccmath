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

#include "ccmath/internal/math/generic/builtins/nearest/ceil.hpp"
#include "ccmath/internal/math/runtime/func/nearest/ceil_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/math/nearest/trunc.hpp"

#include <limits>
#include <type_traits>

namespace ccm
{
	/**
	 * @brief Computes the smallest integer value not less than num.
	 * @tparam T The type of the number.
	 * @param num A floating-point or integer value.
	 * @return The smallest integer value not less than num.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T ceil(T num) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_ceil<T>)
		{
			if (ccm::support::is_constant_evaluated()) { return ccm::builtin::ceil(num); }
		}
		{
			// If num is NaN, NaN is returned.
			// If num is ±∞ or ±0, num is returned, unmodified.
			if (ccm::isinf(num) || num == static_cast<T>(0) || ccm::isnan(num)) { return num; }

			if (!ccm::support::is_constant_evaluated()) { return ccm::rt::ceil_rt(num); }

			const T truncated = ccm::trunc(num);
			if (truncated == num || num < static_cast<T>(0)) { return truncated; }
			return truncated + static_cast<T>(1);
		}
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double ceil(Integer num) noexcept
	{
		return static_cast<double>(num);
	}

	constexpr float ceilf(float num) noexcept
	{
		return ccm::ceil<float>(num);
	}

	constexpr double ceill(double num) noexcept
	{
		return ccm::ceil<double>(num);
	}
} // namespace ccm

/// @ingroup nearest
