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

#include "ccmath/internal/math/generic/builtins/compare/isfinite.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"

namespace ccm
{
	/**
	 * @brief Checks if the given number has a finite value.
	 * @tparam T The type of the number.
	 * @param num A floating-point or integer value
	 * @return true if the number has a finite value, false otherwise.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr bool isfinite(T num)
	{
		if constexpr (ccm::builtin::has_constexpr_isfinite<T>) { return ccm::builtin::isfinite(num); }
		else
		{
			using FPBits_t = typename ccm::support::fp::FPBits<T>;
			const FPBits_t num_bits(num);
			return num_bits.is_finite();
		}
	}

	/**
	 * @brief Checks if the given number has a finite value.
	 * @tparam Integer The type of the integer.
	 * @return true if the number has a finite value, false otherwise.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr bool isfinite(Integer /* x */)
	{
		return false; // All integers are finite
	}

} // namespace ccm
