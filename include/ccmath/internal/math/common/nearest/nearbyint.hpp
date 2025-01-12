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

#include <ccmath/internal/support/fenv/rounding_mode.hpp>
#include <ccmath/internal/support/fp/directional_rounding_utils.hpp>
#include <ccmath/math/compare/isinf.hpp>
#include <ccmath/math/compare/isnan.hpp>
#include <ccmath/math/compare/signbit.hpp>
#include <ccmath/math/nearest/trunc.hpp>
#include <type_traits>

namespace ccm
{
	/**
	 * @brief The nearest integer value to num, according to the rounding mode FE_TONEAREST, is returned.
	 * @tparam T The type of the number.
	 * @param num A floating-point value.
	 * @return If no errors occur, the rounded floating point value. Otherwise, returns the input floating point value unmodified.
	 */
	template <class T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T nearbyint(T num) noexcept
	{
		constexpr auto rounding_mode{ccm::support::fenv::get_rounding_mode()};
		return ccm::support::fp::directional_round(num, rounding_mode);
	}

	/**
	 * @brief The nearest integer value to num, according to the rounding mode FE_TONEAREST, is returned.
	 * @tparam Integer The type of the number.
	 * @param num An integral value.
	 * @return If no errors occur, the rounded floating point value. Otherwise, returns the input integer value unmodified.
	 */
	template <class Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double nearbyint(Integer num) noexcept
	{
		return static_cast<double>(num);
	}

	/**
	 * @brief The nearest integer value to num, according to the rounding mode FE_TONEAREST, is returned.
	 * @param num A float value.
	 * @return If no errors occur, the rounded floating point value. Otherwise, returns the input integer value unmodified.
	 */
	constexpr float nearbyintf(float num) noexcept
	{
		return ccm::nearbyint(num);
	}

	/**
	 * @brief The nearest integer value to num, according to the rounding mode FE_TONEAREST, is returned.
	 * @param num A long double value.
	 * @return If no errors occur, the rounded floating point value. Otherwise, returns the input integer value unmodified.
	 */
	constexpr long double nearbyintl(long double num) noexcept
	{
		return ccm::nearbyint(num);
	}

} // namespace ccm
