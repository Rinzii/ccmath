/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/detail/compare/isnan.hpp"
#include "ccmath/detail/compare/isinf.hpp"
#include "ccmath/detail/basic/remainder.hpp"

namespace ccm
{
	template<typename T>
    inline constexpr T remquo(T x, T y, int* quo)
    {
#if false && (defined(__GNUC__) && !defined(__clang__) && __GNUC__ >= 7 && __GNUC_MINOR__ >= 1)
        // Works with GCC 7.1
        // Not static_assert-able
        return __builtin_remquo(x, y, quo);
#else
		// TODO: This function is a lot trickier to get working with constexpr.
		// TODO: I'm putting this on hold for now and not require it for the first release.
		if constexpr (std::is_floating_point_v<T>)
		{
			// If x is ±∞ and y is not NaN, NaN is returned.
			// If y is ±0 and x is not NaN, NaN is returned.
			// If either argument is NaN, NaN is returned.
			if ((ccm::isinf(x) && !ccm::isnan(y)) || (y == 0 && !ccm::isnan(x)) || (ccm::isnan(x) || ccm::isnan(y)))
			{
				// All major compilers return -NaN.
				return -std::numeric_limits<T>::quiet_NaN();
			}
		}

		T r = ccm::remainder(x, y);
		// Having a lot of issues with handling the quo parameter. May use __builtin_bit_cast to handle this.
		//*quo = static_cast<int>(x / y) & ~(std::numeric_limits<int>::min)();

		return r;
#endif
    }

} // namespace ccm
