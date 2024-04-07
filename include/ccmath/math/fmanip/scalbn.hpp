/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/fmanip/impl/scalbn_double_impl.hpp"
#include "ccmath/math/fmanip/impl/scalbn_float_impl.hpp"
#include "ccmath/math/fmanip/impl/scalbn_ldouble_impl.hpp"

namespace ccm
{
	template <typename T>
	constexpr T scalbn(T x, int n) noexcept
	{
		if constexpr (std::is_same_v<T, float>) { return ccm::internal::impl::scalbn_float_impl(x, n); }
		if constexpr (std::is_same_v<T, long double>) { return ccm::internal::impl::scalbn_ldouble_impl(x, n); }
		return static_cast<T>(ccm::internal::impl::scalbn_double_impl(x, n));
	}

} // namespace ccm
