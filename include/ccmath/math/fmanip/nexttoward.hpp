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


#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T nexttoward(T from, long double to) noexcept
	{
		// TODO: Better define how this interacts with the builtin.
		if constexpr (ccm::builtin::has_constexpr_nexttoward<T>) { return ccm::builtin::nexttoward(from, to); }
		else { return gen::nextafter_gen(from, to); }
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double nexttoward(Integer from, long double to) noexcept
	{
		return gen::nextafter_gen(from, to);
	}

	constexpr float nexttowardf(float from, long double to) noexcept
	{
		return gen::nextafter_gen(from, to);
	}

	constexpr long double nexttowardl(long double from, long double to) noexcept
	{
		return gen::nextafter_gen(from, to);
	}

} // namespace ccm
