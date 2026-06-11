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

#include "ccmath/internal/math/generic/builtins/nearest/round.hpp"
#include "ccmath/internal/math/runtime/func/nearest/round_rt.hpp"
#include "ccmath/internal/support/fp/directional_rounding_utils.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/isnan.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Rounds num to the nearest integer, with halfway cases away from zero.
	 * @tparam T The type of the number.
	 * @param num A floating-point or integer value.
	 * @return The rounded value.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/round
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T round(T num) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_round<T>)
		{
			if (ccm::support::is_constant_evaluated()) { return ccm::builtin::round_ct(num); }
		}
		{
			// If num is NaN, NaN is returned.
			// If num is ±∞ or ±0, num is returned, unmodified.
			if (ccm::isinf(num) || num == static_cast<T>(0) || ccm::isnan(num)) { return num; }

			if (!ccm::support::is_constant_evaluated()) { return ccm::rt::round_rt(num); }

			constexpr int round_mode = static_cast<int>(ccm::support::fp::rounding_mode::eFE_TONEARESTFROMZERO);
			return ccm::support::fp::directional_round(num, round_mode);
		}
	}

	/**
	 * @brief Returns the rounded value of an integer input after conversion to double.
	 * @tparam Integer Integral type.
	 * @param num Integer value.
	 * @return num converted to double.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/round
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double round(Integer num) noexcept
	{ return static_cast<double>(num); }

	/**
	 * @brief Rounds a float to the nearest integer value, halfway cases away from zero.
	 * @param num Floating-point value.
	 * @return Rounded value as float.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/round
	 */
	constexpr float roundf(float num) noexcept
	{ return ccm::round<float>(num); }

	/**
	 * @brief Rounds a double to the nearest integer value, halfway cases away from zero.
	 * @param num Floating-point value.
	 * @return Rounded value as double.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/round
	 */
	constexpr long double roundl(long double num) noexcept
	{ return ccm::round<long double>(num); }
} // namespace ccm

/// @ingroup nearest
