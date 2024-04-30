/*
* Copyright (c) 2024-Present Ian Pike
* Copyright (c) 2024-Present cmath contributors
*
* This library is provided under the MIT License.
* See LICENSE for more information.
*/

#pragma once

namespace ccm::internal::impl
{
	// round to odd function
	constexpr double round_to_odd(double x) noexcept
	{
		// get the integer part of x
		double int_part = 0.0;
		double frac_part = std::modf(x, &int_part);

		// if the integer part is even, add 1 to it
		if (static_cast<int>(int_part) % 2 == 0)
		{
			int_part += 1.0;
		}

		// return the sum of the integer part and the fractional part
		return int_part + frac_part;
	}

	constexpr float fma_float_impl(float x, float y, float z) noexcept
	{
		double d {};

		double temp = static_cast<double>(x) * static_cast<double>(y);

		if (temp == -z)
		{
			return static_cast<float>(temp + static_cast<double>(z));
		}

		// perform addition with round to odd


		d = temp + static_cast<double>(z);





		return static_cast<float>(temp + static_cast<double>(z));
	}

} // namespace ccm::internal::impl