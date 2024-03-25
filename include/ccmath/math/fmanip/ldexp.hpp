/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <ccmath/internal/support/bits.hpp>
#include "ccmath/internal/support/floating_point_traits.hpp" //this is included in bits, but i'm going to include it anyways

namespace ccm
{
	// this function multiples parameter x by two to the power of paramter powerOf2
	// x * 2^powerOf2
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	inline constexpr T ldexp(T x, int32_t powerOf2) noexcept
	{
		if (!ccm::isfinite(x)) { return x; }
		int32_t oldexp = ccm::helpers::get_exponent_of_floating_point<T>(x);

		// if the mantissa is 0 and the original exponent is 0
		if ((oldexp == 0) && ((ccm::helpers::bit_cast<ccm::helpers::float_bits_t<T>>(x) &
							   ccm::helpers::floating_point_traits<T>::normal_mantissa_mask) == 0))
		{
			return x;
		}

		if (powerOf2 > ccm::helpers::floating_point_traits<T>::maximum_binary_exponent)
		{
			// error == hugeval
			return std::numeric_limits<T>::infinity();
		}
		// the reference source says -2 * exp_max
		else if (powerOf2 < ccm::helpers::floating_point_traits<T>::minimum_binary_exponent)
		{
			// error == range
			return 0;
		}
		// normalizes an abnormal floating point
		if (oldexp == 0)
		{
			x *= ccm::helpers::floating_point_traits<T>::normalize_factor;
			powerOf2 = -sizeof(T) * 8; //8 is bits in a byte
			oldexp	 = ccm::helpers::get_exponent_of_floating_point<T>(x);
		}

		powerOf2 = oldexp + powerOf2;
		if (powerOf2 >= ccm::helpers::floating_point_traits<T>::maximum_binary_exponent)
		{
			// overflow
			return std::numeric_limits<T>::infinity();
		}
		if (powerOf2 > 0)
		{
			return ccm::helpers::set_exponent_of_floating_point<T>(x, powerOf2);
		}
		// denormal, or underflow
		powerOf2 += sizeof(T) * 8; //8 is bits in a byte
		x = ccm::helpers::set_exponent_of_floating_point<T>(x, powerOf2);
		x /= ccm::helpers::floating_point_traits<T>::normalize_factor;
		if (x == static_cast<T>(0))
		{
			// underflow report
		}
		return x;
	}

	template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	inline constexpr double ldexp(T x, int32_t powerOf2)
	{
		return ldexp(static_cast<double>(x), powerOf2);
	}
} // namespace ccm
