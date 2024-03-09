/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <limits>
#include <type_traits>

namespace ccm::helpers
{
	template <typename T>
	constexpr T is_odd(const T val) noexcept
	{
		return val % static_cast<T>(2) != static_cast<T>(0);
	}

	template <typename T, typename U>
	constexpr T compute_integral_pow(T base, U exponent) noexcept;

	// We calculate integral powers using exponentiation by squaring.
	// https://en.wikipedia.org/wiki/Exponentiation_by_squaring
	template <typename T, typename U>
	constexpr T compute_integral_pow_recursive(const T base, const T val, const U exponent) noexcept
	{
		if (exponent > static_cast<U>(1))
		{
			if (is_odd(exponent)) { return compute_integral_pow_recursive(base * base, val * base, exponent / static_cast<U>(2)); }
			else { return compute_integral_pow_recursive(base * base, val, exponent / static_cast<U>(2)); }
		}
		else if (exponent == static_cast<U>(1)) { return val * base; }
		else { return val; }
	}

	template <typename T, typename U, typename std::enable_if<std::is_signed_v<U>>::value * = nullptr>
	constexpr T check_sign_integral_pow(const T base, const U exponent) noexcept
	{
		if (exponent < static_cast<U>(0)) { return static_cast<T>(1) / compute_integral_pow(base, -exponent); }
		else { return compute_integral_pow_recursive(base, static_cast<T>(1), exponent); }
	}

	template <typename T, typename U, typename std::enable_if<!std::is_signed_v<U>>::value * = nullptr>
	constexpr T check_sign_integral_pow(const T base, const U exponent) noexcept
	{
		return compute_integral_pow_recursive(base, static_cast<T>(1), exponent);
	}

	template <typename T, typename U>
	constexpr T compute_integral_pow(T base, U exponent) noexcept
	{
		if (exponent == static_cast<U>(0)) { return static_cast<T>(1); }

		if (exponent == static_cast<U>(1)) { return base; }

		if (exponent == static_cast<U>(2)) { return base * base; }

		if (exponent == static_cast<U>(3)) { return base * base * base; }

		if (exponent == std::numeric_limits<U>::min()) { return static_cast<T>(0); }

		if (exponent == std::numeric_limits<U>::max()) { return std::numeric_limits<T>::infinity(); }

		return check_sign_integral_pow(base, exponent);
	}

	template <typename T, typename U, typename std::enable_if<std::is_integral_v<U>>::value * = nullptr>
	constexpr T type_check_integral_pow(const T base, const U exponent) noexcept
	{
		return compute_integral_pow(base, exponent);
	}

	template <typename T, typename U, typename std::enable_if<!std::is_integral_v<U>>::value * = nullptr>
	constexpr T type_check_integral_pow(const T base, const U exponent) noexcept
	{
		return compute_integral_pow(base, static_cast<long long int>(exponent));
	}

	template <typename T, typename U>
	constexpr T pow_integral(const T base, const U exponent) noexcept
	{
		return ccm::helpers::type_check_integral_pow(base, exponent);
	}

} // namespace ccm::helpers
