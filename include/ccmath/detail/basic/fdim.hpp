/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <type_traits>
#include <limits>

#include "ccmath/detail/compare/isnan.hpp"

namespace ccm
{
	namespace
	{
		namespace impl
		{
			template <typename T>
            inline constexpr T fdim_impl(T x, T y)
            {
				if constexpr (std::is_floating_point_v<T>)
                {
					if (ccm::isnan(x)) { return x; }
					if (ccm::isnan(y)) { return y; }
				}

				if (x <= y)
				{
					return T(+0.0);
                }
				else if ((y < T(0.0)) && (x > (std::numeric_limits<T>::max() + y)))
                {
					return std::numeric_limits<T>::infinity();
				}
				else
                {
					return x - y;
				}
            }

		}
	}

	template <typename Real, std::enable_if_t<std::is_floating_point<Real>::value, int> = 0>
	inline constexpr Real fdim(Real x, Real y)
    {
		return impl::fdim_impl(x, y);
    }

	template <typename T1, typename T2, std::enable_if_t<std::is_floating_point<T1>::value && std::is_floating_point<T2>::value, int> = 0>
	inline constexpr auto fdim(T1 x, T2 y)
    {

		// Find the common type of the two arguments
		using shared_type = std::common_type_t<T1, T2>;

		// Convert the arguments to the common type
		return fdim(static_cast<shared_type>(x), static_cast<shared_type>(y));
    }

	template <typename Integer, std::enable_if_t<std::is_integral<Integer>::value, int> = 0>
	inline constexpr double fdim(Integer x, Integer y)
    {
        return fdim(static_cast<double>(x), static_cast<double>(y));
    }

	inline constexpr float fdimf(float x, float y)
    {
        return fdim(x, y);
    }

	inline constexpr long double fdiml(long double x, long double y)
    {
        return fdim(x, y);
    }

} // namespace ccm
