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
#include "ccmath/math/fmanip/impl/modf_impl.hpp"

#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T modf(T x, T * iptr) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_modf<T>) { return ccm::builtin::modf_ct(x, iptr); }
		else if constexpr (std::is_same_v<T, float>) { return internal::impl::modf_impl(x, iptr); } // NOLINT(bugprone-branch-clone)
		else if constexpr (std::is_same_v<T, double>) { return internal::impl::modf_impl(x, iptr); }
		else
		{
			double integer_part{};
			const double fractional = internal::impl::modf_impl(static_cast<double>(x), &integer_part);
			*iptr					= static_cast<T>(integer_part);
			return static_cast<T>(fractional);
		}
	}

	constexpr float modff(float x, float * iptr) noexcept
	{ return ccm::modf(x, iptr); }

	constexpr long double modfl(long double x, long double * iptr) noexcept
	{ return ccm::modf(x, iptr); }
} // namespace ccm
