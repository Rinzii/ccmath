/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/detail/compare/isnan.hpp"

#include <type_traits>
#include <limits>
#include <cstdint>

namespace ccm
{
	namespace
	{
		namespace impl
		{
			constexpr std::size_t defaultMaxIterations = 100000;

			//fast_log abs(rel) : avgError = 2.85911e-06(3.32628e-08), MSE = 4.67298e-06(5.31012e-08), maxError = 1.52588e-05(1.7611e-07)
			const double s_log_C0 = -19.645704f;
			const double s_log_C1 = 0.767002f;
			const double s_log_C2 = 0.3717479f;
			const double s_log_C3 = 5.2653985f;
			const double s_log_C4 = -(1.0f + s_log_C0) * (1.0f + s_log_C1) / ((1.0f + s_log_C2) * (1.0f + s_log_C3)); //ensures that log(1) == 0
			const double s_log_2 = 0.6931472f;

			// This is a fast approximation of the natural log. It is not as accurate as the Taylor series expansion, but it is much faster.
			// Still, it is accurate enough for most purposes.
            [[nodiscard]] inline double fast_log(double val) noexcept
			{
				// 0x3ff0000000000000 is the bit representation of 1.0
				// 52 is the number of bits in the mantissa
				constexpr long double oneBit = 0x3ff0000000000000;
				constexpr int bitsInMantissaMinusSign = 52;

				auto ux = __builtin_bit_cast(std::uint64_t, val);
				std::uint64_t exponent = static_cast<std::uint64_t>(ux - oneBit) >> bitsInMantissaMinusSign; //e = exponent part can be negative
				ux |= 0x3ff0000000000000;
				ux &= 0x3fffffffe0000000; // 1 <= x < 2  after replacing the exponent field


				val = __builtin_bit_cast(double, ux);
				double a = (val + s_log_C0) * (val + s_log_C1);
				double b = (val + s_log_C2) * (val + s_log_C3);
				double c = (static_cast<double>(exponent) + s_log_C4);
				double d = a / b;
				return (c + d) * s_log_2;
			}

			[[nodiscard]] inline constexpr long double remez_log(long double num) noexcept
            {
				return static_cast<long double>(0);
			}

			// This is the Taylor series expansion of the natural log with the ability to specify the number of iterations e.g. precision.
			// TODO: Find a faster method for calculating the natural log that is still accurate.
            [[nodiscard]] inline constexpr long double taylor_log(long double num, std::size_t maxIterations) noexcept
			{
				long double ratio = (num - static_cast<long double>(1)) / (num + static_cast<long double>(1));

				long double accumulation = ratio;
				long double total = accumulation;

				std::size_t power = 3;
				while (power < maxIterations)
				{
					accumulation *= ratio;
					accumulation *= ratio;
					total += (1 / static_cast<long double>(power)) * accumulation;
					power += 2;
				}

				return static_cast<long double>(2) * total;
			}


			template <typename T>
			[[nodiscard]] inline constexpr T log_impl(T num, std::size_t maxIterations, bool enableFastLog) noexcept
            {
				if (num <= static_cast<T>(0)) { return -std::numeric_limits<T>::infinity(); }

				if (num == static_cast<T>(1)) { return static_cast<T>(0); }

				if (num < static_cast<T>(0)) { return std::numeric_limits<T>::quiet_NaN(); }

				if (num == std::numeric_limits<T>::infinity()) { return std::numeric_limits<T>::infinity(); }

				if (ccm::isnan(num)) { return std::numeric_limits<T>::quiet_NaN(); }

				// Convert our number to a long double to get more precision.
				if (enableFastLog)
                {
					return static_cast<T>(fast_log(static_cast<float>(num)));
				}
				else
                {
					return static_cast<T>(taylor_log(static_cast<long double>(num), maxIterations));
				}
            }
		}
	}

	/**
     * @brief Computes the natural logarithm of a number.
     * @tparam T The type of the number.
     * @param num A floating-point or integer value
     * @param maxIterations The number of iterations to use in the Taylor series expansion. Higher values will result in more precision.
     * @return The natural logarithm of the number.
     */
	template <typename T>
    [[nodiscard]] inline constexpr T log(T num, std::size_t maxIterations) noexcept
    {
		return impl::log_impl(num, maxIterations, false);
    }

	/**
     * @brief Computes the natural logarithm of a number.
     * @tparam T The type of the number.
     * @param num A floating-point or integer value
     * @param maxIterations The number of iterations to use in the Taylor series expansion. Higher values will result in more precision.
     * @return The natural logarithm of the number.
	 */
	template <typename T>
	[[nodiscard]] inline constexpr T log(T num, bool enableFastLog = true) noexcept
	{
		return impl::log_impl(num, impl::defaultMaxIterations, enableFastLog);
	}
} // namespace ccm
