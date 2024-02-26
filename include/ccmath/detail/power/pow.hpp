/*
* Copyright (c) 2024-Present Ian Pike
* Copyright (c) 2024-Present cmath contributors
*
* This library is provided under the MIT License.
* See LICENSE for more information.
*/

#pragma once

namespace ccm
{
	namespace
	{
		namespace detail
		{
			template <typename T>
			constexpr T pow_dbl(const T base, const T exp) noexcept
            {
                return exp == 0 ? 1 : base * pow_dbl(base, exp - 1);
            }
	}

	template <typename T>
	constexpr T pow(T x)
	{
		return x * x;
	}

}




