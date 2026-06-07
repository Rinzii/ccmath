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

#include "ccmath/math/basic/abs.hpp"
#include "ccmath/math/basic/signbit.hpp"

#include <type_traits>

namespace ccm::ext
{
	/**
	 * @brief Returns x with the sign of y.
	 *
	 * @tparam T Floating-point type of the input and output.
	 * @param x Value whose magnitude is used.
	 * @param y Value whose sign is used.
	 * @return x with the sign of y.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] constexpr T chgsign(T x, T y) noexcept
	{
		const T magnitude = ::ccm::abs(x);

		if (::ccm::signbit(y)) {
			return -magnitude;
		}

		return magnitude;
	}
} // namespace ccm::ext