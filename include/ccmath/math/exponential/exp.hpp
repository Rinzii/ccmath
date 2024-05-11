/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present cmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/exponential/impl/exp_double_impl.hpp"
#include "ccmath/math/exponential/impl/exp_float_impl.hpp"

namespace ccm
{
	/**
	 * @brief Computes e raised to the given power
	 * @tparam T floating-point or integer type
	 * @param num floating-point or integer value
	 * @return If no errors occur, the base-e exponential of num (e^num) is returned.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T exp(T num)
	{
#if defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__)
		if constexpr (std::is_same_v<T, float>) { return __builtin_expf(num); }
		if constexpr (std::is_same_v<T, double>) { return __builtin_exp(num); }
		if constexpr (std::is_same_v<T, long double>) { return __builtin_expl(num); }
		return static_cast<T>(__builtin_expl(num));
#else
		if constexpr (std::is_same_v<T, float>) { return internal::impl::exp_float_impl(num); }
		if constexpr (std::is_same_v<T, double>) { return internal::impl::exp_double_impl(num); }
		if constexpr (std::is_same_v<T, long double>) { return static_cast<long double>(internal::impl::exp_double_impl(static_cast<double>(num))); }
		return static_cast<T>(internal::impl::exp_double_impl(static_cast<double>(num)));
#endif
	}

	/**
	 * @brief Computes e raised to the given power
	 * @tparam Integer integer type
	 * @param num integer value
	 * @return If no errors occur, the base-e exponential of num (e^num) is returned as double.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double exp(Integer num)
	{
		return ccm::exp<double>(static_cast<double>(num));
	}

	/**
	 * @brief Computes e raised to the given power
	 * @param num floating-point value
	 * @return If no errors occur, the base-e exponential of num (e^num) is returned as float.
	 */
	constexpr float expf(float num)
	{
		return ccm::exp<float>(num);
	}

	/**
	 * @brief Computes e raised to the given power
	 * @param num floating-point value
	 * @return If no errors occur, the base-e exponential of num (e^num) is returned as double.
	 */
	constexpr long double expl(long double num)
	{
		return ccm::exp<long double>(num);
	}

} // namespace ccm
