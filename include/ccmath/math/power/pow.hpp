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
#include "ccmath/internal/support/is_constant_evaluated.hpp"

#include <type_traits>

namespace ccm
{

	template <typename T>
	constexpr T pow(T base, T exp) noexcept
	{
		// TODO: Add in usage of builtins that meet ccmath standards.

		if (support::is_constant_evaluated())
		{
			return gen::pow_gen(base, exp);
		}

		return gen::pow_gen(base, exp); // TODO: Replace once runtime implementation is hooked in.
	}

} // namespace ccm
