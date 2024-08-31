/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/basic/fma.hpp"

namespace ccm
{
	template <typename T>
	constexpr T lerp(T a, T b, T t) noexcept
	{
		// Optimized version of lerp
		// https://developer.nvidia.com/blog/lerp-faster-cuda/
		// TODO: Validate this works for all cases of a lerp.
		return ccm::fma(t, b, ccm::fma(-t, a, a));
	}

	template <typename T, typename U, typename V>
	constexpr std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U> && std::is_arithmetic_v<V>, std::common_type_t<T, U, V>>
	lerp(T a, U b, V t) noexcept
	{
		using result_type = std::common_type_t<T, U, V>;
		static_assert(!(std::is_same_v<T, result_type> && std::is_same_v<U, result_type> && std::is_same_v<V, result_type>));
		return lerp(static_cast<result_type>(a), static_cast<result_type>(b), static_cast<result_type>(t));
	}

	// TODO: Remove this once we confirm the new lerp is 100% stable
	template <typename T>
	[[maybe_unused]] [[deprecated(
		"Do not use ccm::lerp_old it is only being kept as a fallback until ccm::lerp has been validated as conforming to std::lerp")]] constexpr T
	old_lerp(T a, T b, T t) noexcept
	{
		if ((a <= 0 && b >= 0) || (a >= 0 && b <= 0)) { return t * b + (1 - t) * a; }

		if (t == 1) { return b; }

		const T x = a + t * (b - a);
		if ((t > 1) == (b > a)) { return b < x ? x : b; }

		return x < b ? x : b;
	}
} // namespace ccm
