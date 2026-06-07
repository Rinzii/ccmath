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
#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/isnan.hpp"

#include <limits>
#include <type_traits>

namespace ccm
{
	namespace internal::impl
	{
		template <typename T>
		constexpr T floor_pos_impl(T num) noexcept
		{
			// At or above 1/epsilon, num is already integral (or indistinguishable from one) in T.
			constexpr auto max_comparable_val = T(1) / std::numeric_limits<T>::epsilon();

			if (num >= max_comparable_val) { return num; }

			T result = 1;

			if (result == num) { return num; }

			// Bracket num by doubling from 1, then step down to the floor.
			while (result < num) { result *= 2; }

			while (result > num) { --result; }

			return result;
		}

		template <typename T>
		constexpr T floor_neg_impl(T num) noexcept
		{
			T result = -1;

			if (result > num)
			{
				// Bracket num by doubling from -1, then adjust to the largest integer not greater than num.
				while (result > num) { result *= 2; }
				while (result < num) { ++result; }
				if (result != num) { --result; }
			}

			return result;
		}
	} // namespace internal::impl

	/**
	 * @brief Computes the largest integer value not greater than num.
	 * @tparam T The type of the number.
	 * @param num A floating-point or integer value.
	 * @return If no errors occur, the largest integer value not greater than num, that is ⌊num⌋, is returned.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T floor(T num) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_floor<T>) { return ccm::builtin::floor(num); }
		else
		{
			// If num is NaN, NaN is returned.
			// If num is ±∞ or ±0, num is returned, unmodified.
			if (ccm::isinf(num) || num == static_cast<T>(0) || ccm::isnan(num)) { return num; }

			// TODO: This approach should work with long double perfectly, but is slow.
			//		 at some consider adding a faster approach that is just as consistent.
			if (num > 0) { return internal::impl::floor_pos_impl(num); }
			return internal::impl::floor_neg_impl(num);
		}
	}

	/**
	 * @brief Computes the largest integer value not greater than num.
	 * @param num A integer value.
	 * @return If no errors occur, the largest integer value not greater than num, that is ⌊num⌋, is returned.
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
	 */
	constexpr float floorf(float num) noexcept
	{
		return ccm::floor<float>(num);
	}

	/**
	 * @brief Computes the largest integer value not greater than num.
	 * @param num A floating-point value.
	 * @return If no errors occur, the largest integer value not greater than num, that is ⌊num⌋, is returned.
	 */
	constexpr double floorl(double num) noexcept
	{
		return ccm::floor<double>(num);
	}
} // namespace ccm

/// @ingroup nearest
