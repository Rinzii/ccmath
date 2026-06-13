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
	// Simple wrapper for fma

	template <typename T, bool HasConstexprBuiltinFma = ccm::builtin::has_constexpr_fma<T>>
	struct multiply_add_builtin
	{
		static constexpr T call(T x, T y, T z)
		{
			if constexpr (ccm::builtin::has_runtime_fma<T>)
			{
				if (is_constant_evaluated())
				{
					return (x * y) + z; // We can only hope the compiler optimizes this.
				}

				return ccm::builtin::fma_rt(x, y, z);
			}

			return (x * y) + z;
		}
	};

	template <typename T>
	struct multiply_add_builtin<T, true>
	{
		static constexpr T call(T x, T y, T z) { return ccm::builtin::fma_ct(x, y, z); }
	};

	template <typename T>
	constexpr std::enable_if_t<(sizeof(T) > sizeof(void *)), T> multiply_add(const T &x, const T &y, const T &z)
	{ return (x * y) + z; }

	template <typename T>
	constexpr std::enable_if_t<(sizeof(T) <= sizeof(void *)), T> multiply_add(T x, T y, T z)
	{ return (x * y) + z; }

#ifdef CCMATH_TARGET_CPU_HAS_FMA

	constexpr float multiply_add(float x, float y, float z)
	{ return multiply_add_builtin<float>::call(x, y, z); }

	constexpr double multiply_add(double x, double y, double z)
	{ return multiply_add_builtin<double>::call(x, y, z); }
#endif
} // namespace ccm::support
