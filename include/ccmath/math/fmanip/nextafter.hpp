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

#include "ccmath/internal/math/generic/func/fmanip/nextafter_gen.hpp"
#include "ccmath/internal/math/generic/builtins/fmanip/nextafter.hpp"


#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T nextafter(T from, T to) noexcept
	{
		// TODO: Better define how this interacts with the builtin.
		if constexpr (ccm::builtin::has_constexpr_nextafter<T>) { return ccm::builtin::nextafter(from, to); }
		else { return gen::nextafter_gen(from, to); }
	}

	template <typename Arithmetic1, typename Arithmetic2, std::enable_if_t<std::is_arithmetic_v<Arithmetic1> && std::is_arithmetic_v<Arithmetic2>, bool> = true>
	constexpr auto nextafter(Arithmetic1 from, Arithmetic2 to) noexcept
	{
		return gen::nextafter_gen(from, to);
	}

	constexpr float nextafterf(float from, float to) noexcept
	{
		return ccm::nextafter<float>(from, to);
	}

	constexpr long double nextafterl(long double from, long double to) noexcept
	{
		return ccm::nextafter<long double>(from, to);
	}
} // namespace ccm
