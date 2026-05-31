/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// Parts of this code was borrowed from the LLVM Project,
// under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"

namespace ccm::support::fp
{
	constexpr float nearest_integer(float x)
	{
		// Maximum float value that can be exactly represented as an integer
		if (constexpr float float_max_exact_int = 0x1p24F; x < float_max_exact_int && x > -float_max_exact_int)
		{
			constexpr float round_offset = 0x1.0p23F; // Offset used for rounding to nearest integer
			// This expression is correct for the default rounding mode, round-to-nearest, tie-to-even.
			const float r = x < 0 ? (x - round_offset) + round_offset : (x + round_offset) - round_offset;

			// If we are not in a constant evaluated context, we can correct for non-default rounding modes at runtime.
			if (!is_constant_evaluated())
			{
				constexpr float round_increment = 1.0F; // Increment for rounding correction
				constexpr float round_threshold = 0.5F; // Threshold for correcting rounding modes at runtime
				const float diff				= x - r;
				// Correct for non-default rounding modes at runtime
				if (CCM_UNLIKELY(diff > round_threshold)) { return r + round_increment; }
				if (CCM_UNLIKELY(diff < -round_threshold)) { return r - round_increment; }
			}
			return r;
		}
		return x;
	}

	constexpr double nearest_integer(double x)
	{
		// Maximum float value that can be exactly represented as an integer
		if (constexpr double float_max_exact_int = 0x1p53; x < float_max_exact_int && x > -float_max_exact_int)
		{
			constexpr double round_offset = 0x1.0p52; // Offset used for rounding to nearest integer
			// This expression is correct for the default rounding mode, round-to-nearest, tie-to-even.
			const double r = x < 0 ? (x - round_offset) + round_offset : (x + round_offset) - round_offset;

			// If we are not in a constant evaluated context, we can correct for non-default rounding modes at runtime.
			if (!is_constant_evaluated())
			{
				constexpr double round_increment = 1.0F; // Increment for rounding correction
				constexpr double round_threshold = 0.5F; // Threshold for correcting rounding modes at runtime
				const double diff				 = x - r;
				// Correct for non-default rounding modes at runtime
				if (CCM_UNLIKELY(diff > round_threshold)) { return r + round_increment; }
				if (CCM_UNLIKELY(diff < -round_threshold)) { return r - round_increment; }
			}
			return r;
		}
		return x;
	}
} // namespace ccm::support::fp