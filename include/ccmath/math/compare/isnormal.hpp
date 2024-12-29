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

#include "ccmath/math/basic/fabs.hpp"
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
