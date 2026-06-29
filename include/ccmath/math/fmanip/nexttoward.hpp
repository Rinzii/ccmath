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

#include "ccmath/internal/math/generic/builtins/fmanip/nexttoward.hpp"
#include "ccmath/internal/math/generic/func/fmanip/nextafter_gen.hpp"
#include "ccmath/internal/math/runtime/func/fmanip/nexttoward_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Returns the next representable value from from toward to using long double direction precision.
	 * @tparam T Floating-point type for the return value and first argument.
	 * @param from Starting value.
	 * @param to Direction target as long double.
	 * @return Next representable value after from toward to.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true> constexpr T nexttoward(T from, long double to) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_nexttoward<T>)
		{
			return ccm::builtin::nexttoward_ct(from, to);
		} else if (ccm::support::is_constant_evaluated())
		{
			return gen::nextafter_gen(from, to);
		} else
		{
			return ccm::rt::nexttoward_rt(from, to);
		}
	}

	/**
	 * @brief Integer overload for stepping toward a long double target.
	 * @tparam Integer Integral type.
	 * @param from Starting integer value.
	 * @param to Direction target as long double.
	 * @return Next representable double value after converting from to floating-point.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true> constexpr double nexttoward(Integer from, long double to) noexcept
	{
		return gen::nextafter_gen(from, to);
	}

	/**
	 * @brief Returns the next representable float from from toward to.
	 * @param from Starting value.
	 * @param to Direction target as long double.
	 * @return Next representable float value.
	 */
	constexpr float nexttowardf(float from, long double to) noexcept
	{
		return gen::nextafter_gen(from, to);
	}

	/**
	 * @brief Returns the next representable long double from from toward to.
	 * @param from Starting value.
	 * @param to Direction target as long double.
	 * @return Next representable long double value.
	 */
	constexpr long double nexttowardl(long double from, long double to) noexcept
	{
		return gen::nextafter_gen(from, to);
	}

} // namespace ccm
