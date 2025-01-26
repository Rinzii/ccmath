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

#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T log10(T num)
	{
#if defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__)
		if constexpr (std::is_same_v<T, float>) { return __builtin_log10f(num); }
		if constexpr (std::is_same_v<T, double>) { return __builtin_log10(num); }
		if constexpr (std::is_same_v<T, long double>) { return __builtin_log10l(num); }
		return static_cast<T>(__builtin_expm1l(num));
#else
		if constexpr (std::is_same_v<T, float>) { return 0; }
		if constexpr (std::is_same_v<T, double>) { return 0; }
		if constexpr (std::is_same_v<T, long double>) { return 0; }
		return 0;
#endif
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double log10(Integer num)
	{
		return ccm::log10<double>(static_cast<double>(num));
	}

	constexpr float log10f(float num)
	{
		return ccm::log10<float>(num);
	}

	constexpr long double log10l(long double num)
	{
		return ccm::log10<long double>(num);
	}
} // namespace ccm
