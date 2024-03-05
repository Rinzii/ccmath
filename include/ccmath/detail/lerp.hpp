/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

namespace ccm
{
	namespace 
	{
		namespace impl
		{
			template <typename Real>
			inline constexpr Real lerp_impl(Real a, Real b, Real dt)
			{
				if ((a <= 0 && b >= 0) || (a >= 0 && b <= 0))
				{
    				return dt * b + (1 - dt) * a;
				}
				
				if (dt == 1)
				{
    				return b;
				}
				
  				const Real x = a + dt * (b - a);
  				if ((dt > 1) == (b > a))
				{
    				return b < x ? x : b;
				}
  				else
				{
    				return x < b ? x : b;
				}
			}
		}
	}

	template <typename T>
	inline constexpr T lerp(T a, T b, T t)
	{
		// TODO: Implement promotion and edge cases. 
		return lerp_impl(a, b, t);
	}
} // namespace ccm
