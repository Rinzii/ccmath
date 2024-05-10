/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/exponential/impl/exp2_double_impl.hpp"
#include "ccmath/math/exponential/impl/exp2_float_impl.hpp"

#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T exp2(T num)
	{
#if defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__)
		if constexpr (std::is_same_v<T, float>) { return __builtin_exp2f(num); }
		if constexpr (std::is_same_v<T, double>) { return __builtin_exp2(num); }
		if constexpr (std::is_same_v<T, long double>) { return __builtin_exp2l(num); }
		return static_cast<T>(__builtin_exp2l(num));
#else
		if constexpr (std::is_same_v<T, float>) { return internal::exp2_float(num); }
		if constexpr (std::is_same_v<T, double>) { return internal::exp2_double(num); }
		if constexpr (std::is_same_v<T, long double>) { return static_cast<long double>(internal::exp2_double(static_cast<double>(num))); }
		return static_cast<T>(internal::exp2_double(static_cast<double>(num)));
#endif
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double exp2(Integer num)
	{
		return ccm::exp2<double>(static_cast<double>(num));
	}

	constexpr float exp2f(float num)
	{
		return ccm::exp2<float>(num);
	}

	constexpr long double exp2l(long double num)
	{
		return static_cast<long double>(ccm::exp2<double>(static_cast<double>(num)));
	}
} // namespace ccm
