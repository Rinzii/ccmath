/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/basic/abs.hpp"
#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/isnan.hpp"

namespace ccm
{
	/**
	 * @brief Checks if the given number has a normal value.
	 * @tparam T The type of the number.
	 * @param num A floating-point or integer value
	 * @return true if the number has a normal value, false otherwise.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr bool isnormal(T num) noexcept
	{
		return num != static_cast<T>(0) && !ccm::isnan(num) && !ccm::isinf(num) && ccm::abs(num) >= std::numeric_limits<T>::min();
	}

	/**
	 * @brief Checks if the given number has a normal value.
	 * @tparam Integer The type of the integer.
	 * @param num A integer value to check.
	 * @return true if the number has a normal value, false otherwise.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr bool isnormal(Integer num) noexcept
	{
		return num != static_cast<Integer>(0);
	}

} // namespace ccm
