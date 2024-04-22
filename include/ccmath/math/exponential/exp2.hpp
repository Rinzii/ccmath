/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

namespace ccm
{
	template <typename T>
	constexpr T exp2(T num)
	{
#if defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__)
		if constexpr (std::is_same_v<T, float>) { return __builtin_expf(num); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_exp(num); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_expl(num); }
		else { return __builtin_exp(num); }
#else // TODO: Implement generic exp2
		if constexpr (std::is_same_v<T, float>) { return 0; }
		else if constexpr (std::is_same_v<T, double>) { return 0; }
		else if constexpr (std::is_same_v<T, long double>) { return 0; }
		else { return 0; }
#endif
	}

} // namespace ccm
