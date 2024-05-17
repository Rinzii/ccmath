/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <type_traits>

namespace ccm::support
{
	// Simple wrapper for FMA. Will use constexpr builtin FMA if available.

	template <typename T>
	constexpr std::enable_if_t<(sizeof(T) > sizeof(void *)), T>
	multiply_add(const T &x, const T &y, const T &z) {
		#if defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__)
		if constexpr (std::is_same_v<T, float>)
		{
			return __builtin_fmaf(x, y, z);
		}
		if constexpr (std::is_same_v<T, double>)
		{
			return __builtin_fma(x, y, z);
		}
		if constexpr (std::is_same_v<T, long double>)
		{
			return __builtin_fmal(x, y, z);
		}

		return static_cast<T>(__builtin_fmal(x, y, z));
		#else
		return x * y + z;
		#endif
	}

	template <typename T>
	constexpr std::enable_if_t<(sizeof(T) <= sizeof(void *)), T>
	multiply_add(T x, T y, T z) {
#if defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__)
	if constexpr (std::is_same_v<T, float>)
	{
		return __builtin_fmaf(x, y, z);
	}
	if constexpr (std::is_same_v<T, double>)
	{
		return __builtin_fma(x, y, z);
	}
	if constexpr (std::is_same_v<T, long double>)
	{
		return __builtin_fmal(x, y, z);
	}

	return static_cast<T>(__builtin_fmal(x, y, z));
	#else
		return x * y + z;
	#endif
	}
} // namespace ccm::support