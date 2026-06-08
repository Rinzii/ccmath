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

#include "ccmath/internal/math/generic/builtins/nearest/floor.hpp"
#include "ccmath/internal/math/runtime/func/nearest/floor_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/math/nearest/trunc.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Computes the largest integer value not greater than num.
	 * @tparam T The type of the number.
	 * @param num A floating-point or integer value.
	 * @return If no errors occur, the largest integer value not greater than num, that is ⌊num⌋, is returned.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/floor
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T floor(T num) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_floor<T>)
		{
			if (ccm::support::is_constant_evaluated()) { return ccm::builtin::floor(num); }
		}
		{
			// If num is NaN, NaN is returned.
			// If num is ±∞ or ±0, num is returned, unmodified.
			if (ccm::isinf(num) || num == static_cast<T>(0) || ccm::isnan(num)) { return num; }

			if (!ccm::support::is_constant_evaluated()) { return ccm::rt::floor_rt(num); }

			const T truncated = ccm::trunc(num);
			if (truncated == num || num > static_cast<T>(0)) { return truncated; }
			return truncated - static_cast<T>(1);
		}
	}

	/**
	 * @brief Computes the largest integer value not greater than num.
	 * @param num A integer value.
	 * @return If no errors occur, the largest integer value not greater than num, that is ⌊num⌋, is returned.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/floor
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double floor(Integer num) noexcept
	{
		return static_cast<double>(num); // All integers already have a floor value. Just cast to double and return.
	}

	/**
	 * @brief Computes the largest integer value not greater than num.
	 * @param num A floating-point value.
	 * @return If no errors occur, the largest integer value not greater than num, that is ⌊num⌋, is returned.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/floor
	 */
	constexpr float floorf(float num) noexcept
	{ return ccm::floor<float>(num); }

	/**
	 * @brief Computes the largest integer value not greater than num.
	 * @param num A floating-point value.
	 * @return If no errors occur, the largest integer value not greater than num, that is ⌊num⌋, is returned.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/floor
	 */
	constexpr long double floorl(long double num) noexcept
	{ return ccm::floor<long double>(num); }
} // namespace ccm

/// @ingroup nearest
