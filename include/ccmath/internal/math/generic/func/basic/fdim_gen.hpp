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

#include "ccmath/internal/math/generic/builtins/basic/fdim.hpp"
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"

#include <limits>
#include <type_traits>

namespace ccm::gen
{
	/**
	 * @internal
	 * @brief Computes the positive difference of two floating point values (max(0,x−y))
	 * @tparam T A floating-point type.
	 * @param x A floating-point or integer values
	 * @param y A floating-point or integer values
	 * @return If successful, returns the positive difference between x and y.
	 */
	template <typename T>
	constexpr auto fdim(T x, T y) -> std::enable_if_t<std::is_floating_point_v<T>, T>
	{
		if constexpr (builtin::has_constexpr_fdim<T>) { return builtin::fdim(x, y); }
		else
		{
			using FPBits_t = typename ccm::support::fp::FPBits<T>;
			const FPBits_t x_bits(x);
			const FPBits_t y_bits(y);

			if (CCM_UNLIKELY(x_bits.is_nan())) { return x; }
			if (CCM_UNLIKELY(y_bits.is_nan())) { return y; }
			if (x <= y) { return static_cast<T>(+0.0); }
			if (y < static_cast<T>(0.0) && x > std::numeric_limits<T>::max() + y) { return std::numeric_limits<T>::infinity(); }
			return x - y;
		}
	}
} // namespace ccm::gen
