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

namespace ccm
{
	namespace
    {
        namespace impl
        {
			template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
			inline constexpr T fmod_impl_calculate(T x, T y)
			{
				// Calculate the remainder of the division of x by y.
                return x - (ccm::trunc<T>(x / y) * y);
			}

			template <typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
			inline constexpr T fmod_impl_calculate(T x, T y)
			{
				// Calculate the remainder of the division of x by y.
				// static_cast is required to prevent the compiler from complaining about narrowing with integer types.
				return static_cast<T>(x - (ccm::trunc<T>(x / y) * y));
			}

			// Special case for floating point types
			template <typename T>
			inline constexpr T fmod_impl_check(T x, T y) noexcept
			{
				// NOTE: We do not raise FE_INVALID even is situations where the standard demands it.
				if constexpr (std::numeric_limits<T>::is_iec559)
				{
					// If x is ±0 and y is not zero, ±0 is returned.
					if ((x == static_cast<T>(0.0) || static_cast<T>(x) == static_cast<T>(-0.0)) && (y != static_cast<T>(0.0)))
					{
						// The standard specifies that plus or minus 0 is returned depending on the sign of x.
						if (ccm::signbit(x))
						{
							return -static_cast<T>(0.0);
						}
						else
						{
							return static_cast<T>(0.0);
						}
					}

					// If x is ±∞ and y is not NaN, NaN is returned.
					if ((x == +std::numeric_limits<T>::infinity() || x == -std::numeric_limits<T>::infinity()) && !ccm::isnan(y))
					{
						// For some reason all the major compilers return a negative NaN even though I can't find anywhere
						// in the standard that specifies this. I'm going to follow suit and return a negative NaN for now.
						// Overall, this has little effect on checking for NaN. We only really care for conformance with the standard.
						return -std::numeric_limits<T>::quiet_NaN();
					}

					// If y is ±0 and x is not NaN, NaN is returned.
					if ((y == static_cast<T>(0.0) || static_cast<T>(y) == static_cast<T>(-0.0)) && !ccm::isnan(x))
					{
						// Same issue as before. All major compilers return a negative NaN.
						return -std::numeric_limits<T>::quiet_NaN();
					}

					// If y is ±∞ and x is finite, x is returned.
					if ((y == +std::numeric_limits<T>::infinity() || y == -std::numeric_limits<T>::infinity()) && ccm::isfinite(x))
					{
						return x;
					}

					// If either argument is NaN, NaN is returned.
					if (ccm::isnan(x) || ccm::isnan(y))
					{
						// Same problem as before but this time all major compilers return a positive NaN.
						return std::numeric_limits<T>::quiet_NaN();
					}
				}

				return fmod_impl_calculate(x, y);
			}

			template <typename T, typename U, typename TC = std::common_type_t<T, U>>
			inline constexpr TC fmod_impl_type_check(T x, U y) noexcept
			{
				return fmod_impl_check(static_cast<TC>(x), static_cast<TC>(y));
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
	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, int> = 0>
    inline constexpr Real fmod(Real x, Real y)
    {
        return impl::fmod_impl_check(x, y);
    }

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, int> = 0>
	inline constexpr double fmod(Integer x, Integer y)
	{
		return impl::fmod_impl_type_check(x, y);
	}

	template <typename T, typename U>
	inline constexpr std::common_type_t<T, U> fmod(T x, T y)
	{
		return impl::fmod_impl_type_check(x, y);
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
