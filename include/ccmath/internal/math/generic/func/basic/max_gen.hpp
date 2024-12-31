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

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"

#include <limits>
#include <type_traits>

namespace ccm::gen
{
	/**
	 * @brief Computes the larger of the two values.
	 * @tparam T Type of the values to compare.
	 * @param x Left-hand side of the comparison.
	 * @param y Right-hand side of the comparison.
	 * @return If successful, returns the larger of two floating point values. The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename T>
	constexpr T max(T x, T y) noexcept
	{
		if constexpr (std::is_floating_point_v<T>)
		{
			using FPBits_t = typename ccm::support::fp::FPBits<T>;
			const FPBits_t x_bits(x);
			const FPBits_t y_bits(y);

			const bool x_is_nan = x_bits.is_nan();
			const bool y_is_nan = y_bits.is_nan();

			if (CCM_UNLIKELY(x_is_nan && y_is_nan)) { return std::numeric_limits<T>::quiet_NaN(); }

			if (CCM_UNLIKELY(x_is_nan)) { return y; }

			if (CCM_UNLIKELY(y_is_nan)) { return x; }
		}

		return (x > y) ? x : y;
	}
} // namespace ccm::gen
