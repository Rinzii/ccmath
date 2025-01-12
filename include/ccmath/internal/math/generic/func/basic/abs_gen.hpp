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

// ReSharper disable once CppUnusedIncludeDirective
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"

#include <limits>

namespace ccm::gen
{
	/**
	 * @internal
	 * @brief Computes the absolute value of a number.
	 * @tparam T Numeric type.
	 * @param num Floating-point or integer value.
	 * @return If successful, returns the absolute value of arg (|arg|). The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename T>
	constexpr auto abs(T num) -> std::enable_if_t<std::is_floating_point_v<T> && std::is_signed_v<T>, T>
	{
		using FPBits_t = support::fp::FPBits<T>;
		const FPBits_t num_bits(num);

		// If num is NaN, return a quiet NaN.
		if (CCM_UNLIKELY(num_bits.is_nan())) { return std::numeric_limits<T>::quiet_NaN(); }

		// If num is equal to ±zero, return +zero.
		if (num_bits.is_zero()) { return static_cast<T>(0); }

		// If num is less than zero, return -num, otherwise return num.
		return num < 0 ? -num : num;
	}
} // namespace ccm::gen
