/*
* Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T expm1(T num)
	{
		#if defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__)
		if constexpr (std::is_same_v<T, float>) { return __builtin_expm1f(num); }
		if constexpr (std::is_same_v<T, double>) { return __builtin_expm1(num); }
		if constexpr (std::is_same_v<T, long double>) { return __builtin_expm1l(num); }
		return static_cast<T>(__builtin_expm1l(num));
		#else
		if constexpr (std::is_same_v<T, float>) { return 0; }
		if constexpr (std::is_same_v<T, double>) { return 0; }
		if constexpr (std::is_same_v<T, long double>) { return 0; }
		return 0;
		#endif
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double expm1(Integer num)
	{
		return ccm::expm1<double>(static_cast<double>(num));
	}

	constexpr float expm1f(float num)
	{
		return ccm::expm1<float>(num);
	}

	constexpr long double expm1l(long double num)
	{
		return ccm::expm1<long double>(num);
	}
} // namespace ccm
