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
	constexpr T nextafter(T from, T to) noexcept
	{
		return gen::nextafter_gen(from, to);
	}

	template <typename Arithmetic1, typename Arithmetic2, std::enable_if_t<std::is_arithmetic_v<Arithmetic1> && std::is_arithmetic_v<Arithmetic2>, bool> = true>
	constexpr auto nextafter(Arithmetic1 from, Arithmetic2 to) noexcept
	{
		return gen::nextafter_gen(from, to);
	}

	constexpr float nextafterf(float from, float to) noexcept
	{
		return gen::nextafter_gen(from, to);
	}

	constexpr long double nextafterl(long double from, long double to) noexcept
	{
		return gen::nextafter_gen(from, to);
	}
} // namespace ccm
