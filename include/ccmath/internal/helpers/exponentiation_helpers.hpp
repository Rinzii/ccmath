/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <type_traits>

namespace ccm::helpers
{
	/**
	 * @brief Computes is a value is odd.
	 * @tparam T
	 * @param value
	 * @return
	 */
    template <typename T>
    [[nodiscard]] inline constexpr bool is_odd(T value) noexcept
    {
        return value % 2 != 0;
    }

	/**
     * @brief Computes the exponentiation of a base to a power using the exponentiation by squaring algorithm.
     * @tparam T The type of the base and exponent.
     * @param base The base value.
     * @param exponent The exponent value.
     * @return The result of the exponentiation.
     *
     * @attention This function is only available for signed integral types.
     */
	template <typename T, std::enable_if<std::is_integral_v<T> && std::is_signed_v<T>, bool> = true>
	[[nodiscard]] inline constexpr T exponentiation_by_squaring(T base, T exponent) noexcept
	{
		if (exponent < static_cast<T>(0))
		{
			base = static_cast<T>(1) / base;
			exponent = -exponent;
		}

		if (exponent == static_cast<T>(0)) { return static_cast<T>(1); }

		auto result = static_cast<T>(1);
		while (exponent > static_cast<T>(1))
		{
			if (ccm::helpers::is_odd(exponent))
			{
				result *= base;
				exponent -= static_cast<T>(1);
			}

			base *= base;
			exponent /= static_cast<T>(2);
		}

		return result * base;
	}

	// Implement taylor series
	template <typename T>
    [[nodiscard]] inline constexpr T taylor_series(T x, T n) noexcept
    {
        T result = static_cast<T>(1);
        for (T i = static_cast<T>(1); i < n; ++i)
        {
            result += exponentiation_by_squaring(x, i) / i;
        }
        return result;
    }

} // namespace ccm::helpers
