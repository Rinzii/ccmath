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
	inline constexpr T exp(T num)
	{
		if constexpr (std::is_same_v<T, float>) { return ccm::internal::impl::exp_float_impl(num); }
		else { return ccm::internal::impl::exp_double_impl(num); }
	}

	/**
     * @brief Computes e raised to the given power
     * @tparam Integer integer type
     * @param num integer value
     * @return If no errors occur, the base-e exponential of num (e^num) is returned as double.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	inline constexpr double exp(Integer num)
    {
        return ccm::exp<double>(static_cast<double>(num));
    }

    /**
     * @brief Computes e raised to the given power
     * @param num floating-point value
     * @return If no errors occur, the base-e exponential of num (e^num) is returned as float.
     */
	inline constexpr float expf(float num)
    {
        return ccm::internal::impl::exp_float_impl(num);
    }

	/**
     * @brief Computes e raised to the given power
     * @param num floating-point value
     * @return If no errors occur, the base-e exponential of num (e^num) is returned as double.
     */
	inline constexpr long double expl(long double num)
    {
        return static_cast<long double>(ccm::exp<double>(static_cast<double>(num)));
    }

} // namespace ccm
