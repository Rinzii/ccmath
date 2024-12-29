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

#include "ccmath/internal/math/generic/builtins/fmanip/modf.hpp"

namespace ccm
{
	template <typename T>
	constexpr T modf(T x, T* iptr) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_modf<T>)
		{
			return ccm::builtin::modf(x, iptr);
		}
		else
		{
			return 0;
		}
	}
} // namespace ccm
