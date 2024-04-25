/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

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
			// Calculate the maximum value that can be compared with 'num' for equality.
			constexpr auto max_comparable_val = T(1) / std::numeric_limits<T>::epsilon();

			// If 'num' is greater than or equal to the maximum comparable value,
			// it is already an integer or very close to one, return 'num'.
			if (num >= max_comparable_val) { return num; }

			// Initialize the result to 1, as it represents the smallest positive integer
			// that is not less than 'num'.
			T result = 1;

			// Check if 'num' is equal to the initial result (1).
			// If true, return 'num' as it is already an integer.
			if (result == num) { return num; }

			// If 'num' is greater than 1, loop until 'result' becomes greater than or equal to 'num'.
			// This loop doubles the 'result' in each iteration until it surpasses 'num'.
			while (result < num) { result *= 2; }

			// After the previous loop, 'result' might have become greater than 'num'.
			// To get the largest integer not greater than 'num', decrement 'result' until it becomes less than or equal to 'num'.
			while (result > num) { --result; }

			return result;
		}

		template <typename T>
		constexpr T floor_neg_impl(T num) noexcept
		{
			// Initialize the result to -1, as it represents the largest integer not greater than 'num'.
			T result = -1;

			// If 'num' is less than -1, loop until 'result' becomes less than or equal to 'num'.
			// This loop doubles the 'result' in each iteration until it becomes less than or equal to 'num'.
			if (result > num)
			{
				while (result > num) { result *= 2; }
				// After the previous loop, 'result' might have become less than 'num'.
				// To get the largest integer not greater than 'num' for negative numbers,
				// increment 'result' until it becomes greater than or equal to 'num'.
				while (result < num) { ++result; }
				// If 'result' is not equal to 'num', decrement it to ensure it represents the largest integer not greater than 'num'.
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
		// If num is NaN, NaN is returned.
		// If num is ±∞ or ±0, num is returned, unmodified.
		if (ccm::isinf(num) || num == static_cast<T>(0) || ccm::isnan(num)) { return num; }

		// TODO: This approach should work with long double perfectly, but is slow.
		//		 at some consider adding a faster approach that is just as consistent.
		if (num > 0) { return internal::impl::floor_pos_impl(num); }
		return internal::impl::floor_neg_impl(num);
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
		return floor<float>(num);
	}

	/**
	 * @brief Computes the largest integer value not greater than num.
	 * @param num A floating-point value.
	 * @return If no errors occur, the largest integer value not greater than num, that is ⌊num⌋, is returned.
	 */
	constexpr double floorl(double num) noexcept
	{
		return floor<double>(num);
	}
} // namespace ccm

/// @ingroup nearest
