/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <type_traits>

#include "ccmath/detail/compare/isinf.hpp"
#include "ccmath/detail/compare/isnan.hpp"

namespace ccm
{
    namespace
    {
        namespace impl
		{
			template< typename T>
			inline constexpr T fma_impl_internal(T x, T y, T z) noexcept
            {

				// If the compiler has a builtin, use it
#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER) && !defined(__INTEL_LLVM_COMPILER)
				if constexpr (std::is_same_v<T, float>)
				{
					return __builtin_fmaf(x, y, z);
				}
				else if constexpr (std::is_same_v<T, double>)
				{
					return __builtin_fma(x, y, z);
				}
				else if constexpr (std::is_same_v<T, long double>)
				{
					return __builtin_fmal(x, y, z);
				}
#endif
				// If the compiler doesn't have a builtin, use the following and hope that the compiler is smart enough to optimize it
				return (x * y) + z;
            }

			// Special case for floating point types
			template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
			inline constexpr Real fma_impl_switch(Real x, Real y, Real z) noexcept
            {
				// Handle infinity
				if ((x == Real{0} && ccm::isinf(y)) || (y == Real{0} && ccm::isinf(x)))
				{
					return std::numeric_limits<Real>::quiet_NaN();
				}
				if (x * y == std::numeric_limits<Real>::infinity() && z == -std::numeric_limits<Real>::infinity())
                {
                    return std::numeric_limits<Real>::infinity();
                }

				// Handle NaN
				if (ccm::isnan(x) || ccm::isnan(y))
				{
					return std::numeric_limits<Real>::quiet_NaN();
				}
				if (ccm::isnan(z) && (x * y != 0 * std::numeric_limits<Real>::infinity() || x * y != std::numeric_limits<Real>::infinity() * 0))
                {
                    return std::numeric_limits<Real>::quiet_NaN();
                }

                return fma_impl_internal(x, y, z);
            }

			template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
			inline constexpr Integer fma_impl_switch(Integer x, Integer y, Integer z) noexcept
			{
				return fma_impl_internal(x, y, z);
			}
		}
    }

	template< typename T>
	/**
	 * @brief Fused multiply-add
	 * @tparam T
	 * @param x
	 * @param y
	 * @param z
	 * @return
	 */
	inline constexpr T fma(T x, T y, T z) noexcept
    {
		// the switch decides which implementation to use based on the type
        return impl::fma_impl_switch(x, y, z);
    }

	template <typename T1, typename T2, typename T3>
	inline constexpr auto fma(T1 x, T2 y, T3 z) noexcept
    {
		// If our type is an integer epsilon will be set to 0 by default.
		// Instead, set epsilon to 1 so that our type is always at least the widest floating point type.
		constexpr auto T1Common = std::numeric_limits<T1>::epsilon() > 0 ? std::numeric_limits<T1>::epsilon() : 1;
		constexpr auto T2Common = std::numeric_limits<T2>::epsilon() > 0 ? std::numeric_limits<T2>::epsilon() : 1;
		constexpr auto T3Common = std::numeric_limits<T3>::epsilon() > 0 ? std::numeric_limits<T3>::epsilon() : 1;

		using epsilon_type = std::common_type_t<decltype(T1Common), decltype(T2Common), decltype(T3Common)>;

		using shared_type =
			std::conditional_t<T1Common <= std::numeric_limits<epsilon_type>::epsilon() && T1Common <= T2Common, T1,
            std::conditional_t<T2Common <= std::numeric_limits<epsilon_type>::epsilon() && T2Common <= T1Common, T2,
			std::conditional_t<T3Common <= std::numeric_limits<epsilon_type>::epsilon() && T3Common <= T2Common, T3, epsilon_type>>>;

		return ccm::fma(static_cast<shared_type>(x), static_cast<shared_type>(y), static_cast<shared_type>(z));
    }

	template <typename T1, typename T2, typename T3, std::enable_if_t<std::is_integral_v<T1> && std::is_integral_v<T2> && std::is_integral_v<T3>, bool> = true>
	inline constexpr auto fma(T1 x, T2 y, T3 z) noexcept // Special case for if all types are integers.
	{
		using shared_type = std::common_type_t<T1, T2, T3>;
        return ccm::fma(static_cast<shared_type>(x), static_cast<shared_type>(y), static_cast<shared_type>(z));
	}

	inline constexpr float fmaf(float x, float y, float z) noexcept
    {
        return ccm::fma(x, y, z);
    }

	inline constexpr long double fmal(long double x, long double y, long double z) noexcept
    {
        return ccm::fma(x, y, z);
    }

} // namespace ccm
