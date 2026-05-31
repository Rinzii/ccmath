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
	 * @see https://en.cppreference.com/w/cpp/numeric/math/nextafter
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T nexttoward(T from, long double to) noexcept
	{
		// TODO: Better define how this interacts with the builtin.
		if constexpr (ccm::builtin::has_constexpr_nexttoward<T>) { return ccm::builtin::nexttoward(from, to); }
		else if (ccm::support::is_constant_evaluated()) { return gen::nextafter_gen(from, to); }
		else { return ccm::rt::nexttoward_rt(from, to); }
	}

	/**
	 * @brief Integer overload for stepping toward a long double target.
	 * @tparam Integer Integral type.
	 * @param from Starting integer value.
	 * @param to Direction target as long double.
	 * @return Next representable double value after converting from to floating-point.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/nextafter
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double nexttoward(Integer from, long double to) noexcept
	{
		return gen::nextafter_gen(from, to);
	}

	/**
	 * @brief Returns the next representable float from from toward to.
	 * @param from Starting value.
	 * @param to Direction target as long double.
	 * @return Next representable float value.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/nextafter
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
	 * @see https://en.cppreference.com/w/cpp/numeric/math/nextafter
	 */
	constexpr long double nexttowardl(long double from, long double to) noexcept
	{
		return gen::nextafter_gen(from, to);
	}

} // namespace ccm
