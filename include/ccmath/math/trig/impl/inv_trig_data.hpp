/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// Polynomial data adapted from LLVM-libc (libc/src/__support/math/inv_trigf_utils.h).

#pragma once

#include <array>

namespace ccm::internal::inv_trig_data
{
	inline constexpr double k_pi		= 0x1.921fb54442d18p+1;
	inline constexpr double k_pi_over_2 = 0x1.921fb54442d18p+0;

	// P(x^2) - 1 where P(x^2) approximates asin(x)/x on [0, 0.5].
	inline constexpr std::array<double, 12> k_asin_coeffs = {
		0x1.555555555538p-3,  0x1.333333336fd5bp-4, 0x1.6db6db41ce4bcp-5, 0x1.f1c72c66896dep-6, 0x1.6e89f0a0ac64bp-6,  0x1.1c6c111de4074p-6,
		0x1.c6fa84b5699acp-7, 0x1.8ed60a3e6dd19p-7, 0x1.ab3a090750049p-8, 0x1.405213cd1ef46p-6, -0x1.0a5a381f73f65p-6, 0x1.05985a32a9045p-5,
	};
} // namespace ccm::internal::inv_trig_data
