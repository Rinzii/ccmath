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

#include "ccmath/internal/math/generic/builtins/fmanip/nextafter.hpp"
#include "ccmath/internal/math/generic/func/fmanip/nextafter_gen.hpp"
#include "ccmath/internal/math/runtime/func/fmanip/nextafter_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Returns the next representable floating-point value from from toward to.
	 * @tparam T Floating-point type.
	 * @param from Starting value.
	 * @param to Direction target.
	 * @return Next representable value after from in the direction of to.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true> constexpr T nextafter(T from, T to) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_nextafter<T>)
		{
			return ccm::builtin::nextafter_ct(from, to);
		} else if (ccm::support::is_constant_evaluated())
		{
			return gen::nextafter_gen(from, to);
		} else
		{
			return ccm::rt::nextafter_rt(from, to);
		}
	}

	/**
	 * @brief Returns the next representable value from from toward to after common-type conversion.
	 * @tparam Arithmetic1 Arithmetic type of from.
	 * @tparam Arithmetic2 Arithmetic type of to.
	 * @param from Starting value.
	 * @param to Direction target.
	 * @return Next representable value in the promoted common type.
	 */
	template <typename Arithmetic1, typename Arithmetic2, std::enable_if_t<std::is_arithmetic_v<Arithmetic1> && std::is_arithmetic_v<Arithmetic2>, bool> = true>
	constexpr auto nextafter(Arithmetic1 from, Arithmetic2 to) noexcept
	{
		return gen::nextafter_gen(from, to);
	}

	/**
	 * @brief Returns the next representable float from from toward to.
	 * @param from Starting value.
	 * @param to Direction target.
	 * @return Next representable float value.
	 */
	constexpr float nextafterf(float from, float to) noexcept
	{
		return ccm::nextafter<float>(from, to);
	}

	/**
	 * @brief Returns the next representable long double from from toward to.
	 * @param from Starting value.
	 * @param to Direction target.
	 * @return Next representable long double value.
	 */
	constexpr long double nextafterl(long double from, long double to) noexcept
	{
		return ccm::nextafter<long double>(from, to);
	}
} // namespace ccm
