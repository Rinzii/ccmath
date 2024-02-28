/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/detail/basic/abs.hpp"
#include "ccmath/detail/compare/isnan.hpp"
//#include "ccmath/internal/helpers/narrow_cast.hpp"

#include <limits>
#include <type_traits>

namespace ccm
{
	/// @cond DEV
	namespace
	{
		namespace impl
        {
			// Follows the requirements of std::trunc
			// https://en.cppreference.com/w/cpp/numeric/math/trunc
            template <typename T>
            inline constexpr T trunc_impl(T x) noexcept
            {
                if constexpr (std::numeric_limits<T>::is_iec559)
                {
					if (ccm::isnan(x))
                    {
                        return std::numeric_limits<T>::quiet_NaN();
                    }

                    if (x == std::numeric_limits<T>::infinity() || x == -std::numeric_limits<T>::infinity())
                    {
                        return x;
                    }

					if (x == static_cast<T>(-0.0) || x == static_cast<T>(+0.0))
                    {
                        return x;
                    }
                }

				return static_cast<T>(static_cast<long long>(x));
            }
        } // namespace impl
    } // namespace
	/// @endcond

	/**
	 * @brief Returns the integral value nearest to x with the magnitude of the integral value always less than or equal to x.
	 * @tparam T The type of the input.
	 * @param x The value to truncate.
	 * @return Returns a truncated value.
	 */
	template <typename Real, std::enable_if_t<std::is_floating_point<Real>::value, int> = 0>
	inline constexpr Real trunc(Real x) noexcept
	{
		return impl::trunc_impl(x);
	}

	/**
	 * @brief Returns the integral value nearest to x with the magnitude of the integral value always less than or equal to x.
	 * @tparam Integer The type of the input.
	 * @param x The value to truncate.
	 * @return Returns a truncated value.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral<Integer>::value, int> = 0>
	inline constexpr double trunc(Integer x) noexcept
    {
        return static_cast<double>(x);
    }

	/**
	 * @brief Specialization for float that returns the integral value nearest to x with the magnitude of the integral value always less than or equal to x.
	 * @param x The float to truncate.
	 * @return Returns a truncated float.
	 */
	float truncf(float x) noexcept
	{
        return trunc(x);
    }

	/**
	 * @brief Specialization for long double that returns the integral value nearest to x with the magnitude of the integral value always less than or equal to x.
	 * @param x The long double to truncate.
	 * @return Returns a truncated long double.
	 */
	long double truncl(long double x) noexcept
	{
        return trunc(x);
    }


} // namespace ccm
