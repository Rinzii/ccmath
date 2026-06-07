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

#include "ccmath/math/fmanip/impl/modf_impl.hpp"

#include <type_traits>

namespace ccm::gen
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T modf_gen(T x, T * iptr) noexcept
	{
		if constexpr (std::is_same_v<T, float>) { return internal::impl::modf_impl(x, iptr); }
		else if constexpr (std::is_same_v<T, double>) { return internal::impl::modf_impl(x, iptr); }
		else
		{
			double integer_part{};
			const double fractional = internal::impl::modf_impl(static_cast<double>(x), &integer_part);
			*iptr					= static_cast<T>(integer_part);
			return static_cast<T>(fractional);
		}
	}
} // namespace ccm::gen
