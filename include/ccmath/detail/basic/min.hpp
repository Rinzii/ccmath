/*
* Copyright (c) 2024-Present Ian Pike
* Copyright (c) 2024-Present cmath contributors
*
* This library is provided under the MIT License.
* See LICENSE for more information.
*/

#pragma once

// Protect from any global user defined min macro
// For example, Windows.h defines min and max as macros
#ifdef min
	#undef min
#endif

#include <type_traits>
#include "ccmath/detail/compare/isnan.hpp"


namespace ccm
{
	namespace
	{
		namespace impl
		{
			template <typename T>
            inline constexpr T min_impl(const T x, const T y) noexcept
            {
				// If we are comparing floating point numbers, we need to check for NaN
				if constexpr (std::is_floating_point_v<T>)
				{
					if (ccm::isnan(x))
					{
						return y;
					}
					else if (ccm::isnan(y))
					{
						return x;
					}
				}

                return (x < y) ? x : y;
            }

			template <typename T, typename U>
			inline constexpr auto min_impl(const T x, const U y) noexcept
            {
				// Find the common type of the two arguments
				using shared_type = std::common_type_t<T, U>;

				// Then cast the arguments to the common type and call the single argument version
				return static_cast<shared_type>(min_impl<shared_type>(static_cast<shared_type>(x), static_cast<shared_type>(y)));
            }
		}
	}

	template <typename T>
	/**
	 * @brief Returns the smaller of the two values.
	 */
    inline constexpr T min(const T x, const T y) noexcept
    {
		return impl::min_impl(x, y);
    }

	/**
	 * @brief Returns the smaller of the two floating point values.
	 */
	template <typename Real, std::enable_if_t<!std::is_integral_v<Real>, bool> = true>
    inline constexpr Real fmin(const Real x, const Real y) noexcept
    {
        return impl::min_impl<Real>(x, y);
    }

	/**
	 * @brief Returns the smaller of the two floating point values using the widest floating point type.
	 */
	template <typename T1, typename T2>
	inline constexpr auto fmin(const T1 x, const T2 y) noexcept
    {
        return impl::min_impl(x, y);
    }

	/**
	 * @brief Returns the smaller of the two integer values.
	 */
    template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
    inline constexpr Integer fmin(const Integer x, const Integer y) noexcept
    {
        return impl::min_impl<Integer>(x, y);
    }

} // namespace ccm


