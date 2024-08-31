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

#include "ccmath/internal/support/fp/fp_bits.hpp"

// TODO: Finish implementing.

namespace ccm
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
constexpr T frexp(T x, int &exp) {
		support::fp::FPBits<T> bits(x);
		if (bits.is_inf_or_nan()) {
			return x;
}
		if (bits.is_zero()) {
			exp = 0;
			return x;
		}

	}

} // namespace ccm
