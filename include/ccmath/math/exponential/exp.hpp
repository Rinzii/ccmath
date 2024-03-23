/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present cmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/exponential/impl/exp_double_impl.hpp"
#include "ccmath/math/exponential/impl/exp_float_impl.hpp"

namespace ccm
{
	template <typename T>
	inline constexpr T exp(T x)
	{
		if constexpr (std::is_same_v<T, float>) { return ccm::internal::impl::exp_float_impl(x); }
		else { return ccm::internal::impl::exp_double_impl(x); }
	}

} // namespace ccm
