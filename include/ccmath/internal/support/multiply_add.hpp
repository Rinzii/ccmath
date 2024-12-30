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

#include "ccmath/internal/math/generic/builtins/basic/fma.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"

#include <type_traits>

namespace ccm::support
{
	// Simple wrapper for FMA. Will use constexpr builtin FMA if available. Will also use non-constexpr builtin FMA if available when not in constexpr context.

	template <typename T>
	constexpr std::enable_if_t<(sizeof(T) > sizeof(void *)), T> multiply_add(const T & x, const T & y, const T & z)
	{
		if constexpr (ccm::builtin::has_constexpr_fma<T>) { return ccm::builtin::fma(x, y, z); }
		else if constexpr (ccm::builtin::has_fma<T>)
		{
			if (is_constant_evaluated())
			{
				return (x * y) + z; // We can only hope the compiler optimizes this.
			}

			return ccm::builtin::fma(x, y, z);
		}
		else { return (x * y) + z; }
	}

	template <typename T>
	constexpr std::enable_if_t<(sizeof(T) <= sizeof(void *)), T> multiply_add(T x, T y, T z)
	{
		if constexpr (ccm::builtin::has_constexpr_fma<T>) { return ccm::builtin::fma(x, y, z); }
		else if constexpr (ccm::builtin::has_fma<T>)
		{
			if (is_constant_evaluated())
			{
				return (x * y) + z; // We can only hope the compiler optimizes this.
			}

			return ccm::builtin::fma(x, y, z);
		}
		else { return (x * y) + z; }
	}
} // namespace ccm::support
