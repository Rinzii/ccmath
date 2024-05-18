/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/fmanip/impl/scalbn_double_impl.hpp"
#include "ccmath/math/fmanip/impl/scalbn_float_impl.hpp"
#include "ccmath/math/fmanip/impl/scalbn_ldouble_impl.hpp"

namespace ccm
{
	/**
	 * @brief Multiplies a number by FLT_RADIX raised to a power
	 * @tparam T Floating-point or integer type.
	 * @param num Floating-point or integer value.
	 * @param exp Integer value.
	 * @return If no errors occur, num multiplied by FLT_RADIX to the power of exp (num×FLT_RADIX^exp) is returned.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T scalbn(T num, int exp) noexcept
	{
#if defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__)
		if constexpr (std::is_same_v<T, float>) { return __builtin_scalbnf(num, exp); }
		if constexpr (std::is_same_v<T, double>) { return __builtin_scalbn(num, exp); }
		if constexpr (std::is_same_v<T, long double>) { return __builtin_scalbnl(num, exp); }
		return static_cast<T>(__builtin_scalbnl(num, exp));
#else
		if constexpr (std::is_same_v<T, float>) { return internal::scalbn_float(num, exp); }
		if constexpr (std::is_same_v<T, double>) { return internal::scalbn_double(num, exp); }
		if constexpr (std::is_same_v<T, long double>) { return internal::scalbn_ldouble(num, exp); }
		return static_cast<T>(internal::scalbn_ldouble(num, exp));
#endif
	}

	/**
	 * @brief Multiplies a number by FLT_RADIX raised to a power
	 * @tparam T Floating-point or integer type.
	 * @param num Floating-point or integer value.
	 * @param exp Integer value.
	 * @return If no errors occur, num multiplied by FLT_RADIX to the power of exp (num×FLT_RADIX^exp) is returned.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T scalbn(T num, long exp) noexcept
	{
		#if defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__)
		if constexpr (std::is_same_v<T, float>) { return __builtin_scalbnf(num, static_cast<int>(exp)); }
		if constexpr (std::is_same_v<T, double>) { return __builtin_scalbn(num, static_cast<int>(exp)); }
		if constexpr (std::is_same_v<T, long double>) { return __builtin_scalbnl(num, static_cast<int>(exp)); }
		return static_cast<T>(__builtin_scalbnl(num, static_cast<int>(exp)));
		#else
		if constexpr (std::is_same_v<T, float>) { return internal::scalbn_float(num, exp); }
		if constexpr (std::is_same_v<T, double>) { return internal::scalbn_double(num, exp); }
		if constexpr (std::is_same_v<T, long double>) { return internal::scalbn_ldouble(num, exp); }
		return static_cast<T>(internal::scalbn_ldouble(num, exp));
		#endif
	}

	/**
	 * @brief Multiplies a number by FLT_RADIX raised to a power
	 * @tparam Integer Integer type.
	 * @param num Integer value.
	 * @param exp Integer value.
	 * @return If no errors occur, num multiplied by FLT_RADIX to the power of exp (num×FLT_RADIX^exp) is returned as a double.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double scalbn(Integer num, int exp) noexcept
	{
		return ccm::scalbn<double>(static_cast<double>(num), exp);
	}

	/**
	 * @brief Multiplies a number by FLT_RADIX raised to a power
	 * @tparam Integer Integer type.
	 * @param num Integer value.
	 * @param exp Integer value.
	 * @return If no errors occur, num multiplied by FLT_RADIX to the power of exp (num×FLT_RADIX^exp) is returned as a double.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double scalbn(Integer num, long exp) noexcept
	{
		return ccm::scalbn<double>(static_cast<double>(num), exp);
	}

	/**
	 * @brief Multiplies a number by FLT_RADIX raised to a power
	 * @param num Floating-point value.
	 * @param exp Integer value.
	 * @return If no errors occur, num multiplied by FLT_RADIX to the power of exp (num×FLT_RADIX^exp) is returned as a float.
	 */
	constexpr float scalbnf(float num, int exp) noexcept
	{
		return ccm::scalbn<float>(num, exp);
	}

	/**
	 * @brief Multiplies a number by FLT_RADIX raised to a power
	 * @param num Floating-point value.
	 * @param exp Integer value.
	 * @return If no errors occur, num multiplied by FLT_RADIX to the power of exp (num×FLT_RADIX^exp) is returned as a float.
	 */
	constexpr float scalbnf(float num, long exp) noexcept
	{
		return ccm::scalbn<float>(num, exp);
	}

	/**
	 * @brief Multiplies a number by FLT_RADIX raised to a power
	 * @param num Floating-point value.
	 * @param exp Integer value.
	 * @return If no errors occur, num multiplied by FLT_RADIX to the power of exp (num×FLT_RADIX^exp) is returned as a long double.
	 */
	constexpr long double scalbnl(long double num, int exp) noexcept
	{
		return ccm::scalbn<long double>(num, exp);
	}

	/**
	 * @brief Multiplies a number by FLT_RADIX raised to a power
	 * @param num Floating-point value.
	 * @param exp Integer value.
	 * @return If no errors occur, num multiplied by FLT_RADIX to the power of exp (num×FLT_RADIX^exp) is returned as a long double.
	 */
	constexpr long double scalbnl(long double num, long exp) noexcept
	{
		return ccm::scalbn<long double>(num, exp);
	}
} // namespace ccm
