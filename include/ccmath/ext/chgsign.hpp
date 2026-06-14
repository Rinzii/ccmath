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

#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/floating_point_traits.hpp"
#include "ccmath/math/basic/fabs.hpp"

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
	constexpr T chgsign(T x, T y) noexcept
	{
		const T magnitude = ::ccm::abs(x);
		const auto y_bits = ::ccm::support::bit_cast<::ccm::support::float_bits_t<T>>(y);

		if ((y_bits & ::ccm::support::sign_mask_v<T>) != 0) { return -magnitude; }

		return magnitude;
	}
} // namespace ccm::ext