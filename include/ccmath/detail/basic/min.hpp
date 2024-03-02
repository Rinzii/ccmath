/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present cmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <type_traits>
#include "ccmath/detail/compare/isnan.hpp"

namespace ccm
{
	/// @cond MATH_DETAIL
	// TODO: Move this into the definitions of the functions directly at some point.
	//       Want to make the code base less fragmented if functions are simple enough.
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
					if (ccm::isnan(x)) { return y; }
					else if (ccm::isnan(y)) { return x; }
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
		} // namespace impl
	}	  // namespace
	/// @endcond

	/**
	 * @brief Computes the smaller of the two values.
	 * @tparam T Type of the values to compare.
	 * @param x Left-hand side of the comparison.
	 * @param y Right-hand side of the comparison.
	 * @return If successful, returns the smaller of two floating point values. The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename T>
	inline constexpr T min(const T x, const T y) noexcept
	{
		return impl::min_impl(x, y);
	}

	/**
	 * @brief Computes the smaller of the two values.
	 * @tparam Real Type of of the values to compare.
	 * @param x Left-hand side of the comparison.
	 * @param y Right-hand side of the comparison.
	 * @return If successful, returns the smaller of two floating point values. The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename Real, std::enable_if_t<!std::is_integral_v<Real>, bool> = true>
	inline constexpr Real fmin(const Real x, const Real y) noexcept
	{
		return impl::min_impl<Real>(x, y);
	}

	/**
	 * @brief Computes the smaller of the two values.
	 * @tparam T Left-hand type of the left-hand value to compare.
	 * @tparam U Right-hand type of the right-hand value to compare.
	 * @param x Left-hand side of the comparison.
	 * @param y Right-hand side of the comparison.
	 * @return If successful, returns the smaller of two floating point values. The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename T, typename U>
	inline constexpr auto fmin(const T x, const U y) noexcept
	{
		return impl::min_impl(x, y);
	}

	/**
	 * @brief Computes the smaller of the two values.
	 * @tparam Integer An integral type.
	 * @param x Right-hand side of the comparison.
	 * @param y Left-hand side of the comparison.
	 * @return If successful, returns the smaller of two floating point values. The value returned is exact and does not depend on any rounding modes.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	inline constexpr Integer fmin(const Integer x, const Integer y) noexcept
	{
		return impl::min_impl<Integer>(x, y);
	}
} // namespace ccm

/// @ingroup basic
