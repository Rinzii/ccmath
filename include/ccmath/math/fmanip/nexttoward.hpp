/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/math/generic/func/fmanip/nextafter_gen.hpp"

#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T nexttoward(T from, long double to) noexcept
	{
		return gen::nextafter_gen(from, to);
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
