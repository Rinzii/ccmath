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

#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"
#include "ccmath/internal/math/runtime/func/power/pow_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/internal/math/generic/builtins/power/pow.hpp"


#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T pow(T base, T exp)
	{
		if constexpr (ccm::builtin::has_constexpr_pow<T>) { return ccm::builtin::pow(base, exp); }
		else
		{
			// TODO: Add in usage of builtins that meet ccmath standards.

			if (support::is_constant_evaluated()) { return gen::pow_gen(base, exp); }
			return rt::pow_rt(base, exp);
		}
	}

	template <typename Integer, std::enable_if_t<!std::is_floating_point_v<Integer>, bool> = true>
	constexpr double pow(Integer base, Integer exp)
	{
		// TODO: Add integer specific optimization with exponentiation of a square
		return ccm::pow<double>(static_cast<double>(base), static_cast<double>(exp));
	}
} // namespace ccm
