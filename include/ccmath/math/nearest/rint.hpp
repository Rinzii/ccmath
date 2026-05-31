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

#include <ccmath/internal/support/fenv/fenv_support.hpp>
#include <ccmath/internal/support/fenv/rounding_mode.hpp>
#include <ccmath/internal/support/fp/directional_rounding_utils.hpp>
#include <ccmath/internal/support/is_constant_evaluated.hpp>
#include <ccmath/math/compare/isinf.hpp>
#include <ccmath/math/compare/isnan.hpp>

#include <cfenv>
#include <type_traits>

namespace ccm
{
	/**
	 * @brief Rounds num to an integer using the current rounding mode.
	 * @tparam T The type of the number.
	 * @param num A floating-point or integer value.
	 * @return The rounded value.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T rint(T num) noexcept
	{
		if (!ccm::support::is_constant_evaluated())
		{
			const T result = ccm::support::fp::directional_round(num, ccm::support::fenv::get_rounding_mode());
			if (!ccm::isnan(num) && result != num) { ccm::support::fenv::raise_except_if_required(FE_INEXACT); }
			return result;
		}

		constexpr auto rounding_mode{ ccm::support::fenv::get_rounding_mode() };
		return ccm::support::fp::directional_round(num, rounding_mode);
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double rint(Integer num) noexcept
	{
		return static_cast<double>(num);
	}

	constexpr float rintf(float num) noexcept
	{
		return ccm::rint(num);
	}

	constexpr long double rintl(long double num) noexcept
	{
		return ccm::rint(num);
	}
} // namespace ccm

/// @ingroup nearest
