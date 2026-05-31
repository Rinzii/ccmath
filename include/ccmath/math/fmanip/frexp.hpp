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
#include "ccmath/internal/math/runtime/func/fmanip/frexp_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/fmanip/impl/frexp_impl.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Decomposes a floating-point value into a normalized fraction and a base-2 exponent.
	 * @tparam T Floating-point type.
	 * @param x Floating-point value to decompose.
	 * @param exp Reference that receives the extracted exponent.
	 * @return Normalized fraction in the range [-1, -0.5) or [0.5, 1), or zero when x is zero.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/frexp
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T frexp(T x, int & exp)
	{
		if constexpr (ccm::builtin::has_constexpr_frexp<T>) { return ccm::builtin::frexp(x, &exp); }
		else if (!ccm::support::is_constant_evaluated()) { return ccm::rt::frexp_rt(x, exp); }
		else if constexpr (std::is_same_v<T, float>) { return internal::impl::frexp_impl(x, exp); }
		else if constexpr (std::is_same_v<T, double>) { return internal::impl::frexp_impl(x, exp); }
		else { return static_cast<T>(internal::impl::frexp_impl(static_cast<double>(x), exp)); }
	}

	/**
	 * @brief Decomposes a float into a normalized fraction and a base-2 exponent.
	 * @param x Floating-point value to decompose.
	 * @param exp Reference that receives the extracted exponent.
	 * @return Normalized fraction for x.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/frexp
	 */
	constexpr float frexpf(float x, int & exp)
	{
		return ccm::frexp(x, exp);
	}

	/**
	 * @brief Decomposes a long double into a normalized fraction and a base-2 exponent.
	 * @param x Floating-point value to decompose.
	 * @param exp Reference that receives the extracted exponent.
	 * @return Normalized fraction for x.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/frexp
	 */
	constexpr long double frexpl(long double x, int & exp)
	{
		return ccm::frexp(x, exp);
	}
} // namespace ccm
