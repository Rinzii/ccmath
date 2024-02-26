/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

//#include <type_traits>
#include <limits>



#include "ccmath/detail/compare/isnan.hpp"
#include "ccmath/detail/compare/isfinite.hpp"
#include "ccmath/detail/nearest/trunc.hpp"
#include "ccmath/detail/compare/signbit.hpp"

#include <cmath>


namespace ccm
{
	namespace
    {
        namespace impl
        {
			template <typename T>
			inline constexpr T fmod_impl_internal(T x, T y)
			{
				// Calculate the remainder of the division of x by y.
                return x - (ccm::trunc(x / y) * y);
			}

			// Special case for floating point types
			template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
			inline constexpr Real fmod_impl_switch(Real x, Real y) noexcept
			{
				// NOTE: We do not raise FE_INVALID even is situations where the standard demands it.
				if constexpr (std::numeric_limits<Real>::is_iec559)
				{
					// If x is ±0 and y is not zero, ±0 is returned.
					if ((x == static_cast<Real>(0.0) || static_cast<Real>(x) == static_cast<Real>(-0.0)) && (y != static_cast<Real>(0.0)))
					{
						// The standard specifies that plus or minus 0 is returned depending on the sign of x.
						if (ccm::signbit(x))
						{
							return -Real{0.0};
						}
						else
						{
							return Real{0.0};
						}
					}

					// If x is ±∞ and y is not NaN, NaN is returned.
					if ((x == +std::numeric_limits<Real>::infinity() || x == -std::numeric_limits<Real>::infinity()) && !ccm::isnan(y))
					{
						// The standard specifies that we always return the same sign as x.
						if (ccm::signbit(x))
						{
							return -std::numeric_limits<Real>::quiet_NaN();
						}
						else
						{
							return std::numeric_limits<Real>::quiet_NaN();
						}
					}

					// If y is ±0 and x is not NaN, NaN is returned.
					if ((y == static_cast<Real>(0.0) || static_cast<Real>(y) == static_cast<Real>(-0.0)) && !ccm::isnan(x))
					{
						// The standard specifies that we always return the same sign as x.
						if (ccm::signbit(x))
						{
							return -std::numeric_limits<Real>::quiet_NaN();
						}
						else
						{
							// NOTE: even though the standard specifies that we should return +NaN. On some compilers they return -NaN.
							// We will return +NaN to be consistent with what is specified in the standard, but be
							// aware that this edge case may be different from std::fmod.
							return std::numeric_limits<Real>::quiet_NaN();
						}
					}

					// If y is ±∞ and x is finite, x is returned.
					if ((y == +std::numeric_limits<Real>::infinity() || y == -std::numeric_limits<Real>::infinity()) && ccm::isfinite(x))
					{
						return x;
					}

					// If either argument is NaN, NaN is returned.
					if (ccm::isnan(x) || ccm::isnan(y))
					{
						if (ccm::signbit(x)) // If x is negative then return negative NaN
						{
							return -std::numeric_limits<Real>::quiet_NaN();
						}
						else
						{
							return std::numeric_limits<Real>::quiet_NaN();
						}
					}
				}

				return fmod_impl_internal(x, y);
			}

			template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
			inline constexpr double fmod_impl_switch(Integer x, Integer y) noexcept
			{
				return fmod_impl_switch<double>(static_cast<double>(x), static_cast<double>(y));
			}
        }
    }

	/**
	 * @brief Returns the floating-point remainder of the division operation x/y.
	 * @note Some edge cases where NaN is returned are different from std::fmod due to the standard allowing implementation based returns.
	 * @tparam T A floating-point type.
	 * @param x A floating-point value.
	 * @param y A floating-point value.
	 * @return The floating-point remainder of the division operation x/y.
	 */
	template <typename T>
    inline constexpr T fmod(T x, T y)
    {
        return impl::fmod_impl_switch(x, y);
    }

	inline constexpr float fmodf(float x, float y)
    {
        return fmod<float>(x, y);
    }

	inline constexpr long double fmodl(long double x, long double y)
    {
        return fmod<long double>(x, y);
    }









} // namespace ccm
