/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/nearest/floor.hpp"

namespace ccm
{
	namespace internal::impl
	{
		template <typename T>
		constexpr T ceil(T x) noexcept
		{
			const T result = ccm::floor(x);
			return result + (x != result);
		}
	} // namespace internal::impl

	/**
	 * @brief Computes the smallest integer value not less than num.
	 * @tparam T The type of the number.
	 * @param num A floating-point or integer value.
	 * @return If no errors occur, the smallest integer value not less than num, that is ⌈num⌉, is returned.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T ceil(T num) noexcept
	{
		return ccm::internal::impl::ceil<T>(num);
	}

	/**
	 * @brief Computes the smallest integer value not less than num.
	 * @param num A integer value.
	 * @return If no errors occur, the smallest integer value not less than num, that is ⌈num⌉, is returned.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double ceil(Integer num) noexcept
	{
		return static_cast<double>(num); // An integer is already a correct ceil. Just cast to double and return.
	}

	/**
	 * @brief Computes the smallest integer value not less than num.
	 * @param num A floating-point value.
	 * @return If no errors occur, the smallest integer value not less than num, that is ⌈num⌉, is returned.
	 */
	constexpr float ceilf(float num) noexcept
	{
		return ceil<float>(num);
	}

	/**
	 * @brief Computes the smallest integer value not less than num.
	 * @param num A floating-point value.
	 * @return If no errors occur, the smallest integer value not less than num, that is ⌈num⌉, is returned.
	 */
	constexpr double ceill(double num) noexcept
	{
		return ceil<double>(num);
	}
} // namespace ccm
