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

#include "ccmath/internal/math/generic/builtins/fmanip/frexp.hpp"
#include "ccmath/math/fmanip/impl/frexp_impl.hpp"

#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T frexp(T x, int & exp)
	{
		if constexpr (ccm::builtin::has_constexpr_frexp<T>) { return ccm::builtin::frexp(x, &exp); }
		else if constexpr (std::is_same_v<T, float>) { return internal::impl::frexp_impl(x, exp); }
		else if constexpr (std::is_same_v<T, double>) { return internal::impl::frexp_impl(x, exp); }
		else
		{
			return static_cast<T>(internal::impl::frexp_impl(static_cast<double>(x), exp));
		}
	}

	constexpr float frexpf(float x, int & exp)
	{ return ccm::frexp(x, exp); }

	constexpr long double frexpl(long double x, int & exp)
	{ return ccm::frexp(x, exp); }
} // namespace ccm
