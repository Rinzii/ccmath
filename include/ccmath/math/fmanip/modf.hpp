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
#include "ccmath/internal/math/runtime/func/fmanip/modf_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/fmanip/impl/modf_impl.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Decomposes a floating-point value into fractional and integral parts.
	 * @tparam T Floating-point type.
	 * @param x Floating-point value to split.
	 * @param iptr Pointer that receives the integral part.
	 * @return Fractional part of x, with the same sign as x.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/modf
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T modf(T x, T * iptr) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_modf<T>) { return ccm::builtin::modf_ct(x, iptr); }
		else if (!ccm::support::is_constant_evaluated())
		{
			T integer_part{};
			const T fractional = ccm::rt::modf_rt(x, integer_part);
			*iptr			   = integer_part;
			return fractional;
		}
		else if constexpr (std::is_same_v<T, float>) { return internal::impl::modf_impl(x, iptr); }
		else if constexpr (std::is_same_v<T, double>) { return internal::impl::modf_impl(x, iptr); }
		else
		{
			double integer_part{};
			const double fractional = internal::impl::modf_impl(static_cast<double>(x), &integer_part);
			*iptr					= static_cast<T>(integer_part);
			return static_cast<T>(fractional);
		}
	}

	/**
	 * @brief Decomposes a float into fractional and integral parts.
	 * @param x Floating-point value to split.
	 * @param iptr Pointer that receives the integral part.
	 * @return Fractional part of x.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/modf
	 */
	constexpr float modff(float x, float * iptr) noexcept
	{ return ccm::modf(x, iptr); }

	/**
	 * @brief Decomposes a long double into fractional and integral parts.
	 * @param x Floating-point value to split.
	 * @param iptr Pointer that receives the integral part.
	 * @return Fractional part of x.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/modf
	 */
	constexpr long double modfl(long double x, long double * iptr) noexcept
	{ return ccm::modf(x, iptr); }
} // namespace ccm
