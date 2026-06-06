/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#pragma once

#include "ccmath/internal/math/generic/builtins/power/hypot.hpp"
#include "ccmath/internal/math/runtime/func/power/hypot_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/power/impl/hypot_impl.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Computes the length of the hypotenuse from two coordinates.
	 * @tparam T Floating-point type.
	 * @param x First value.
	 * @param y Second value.
	 * @return sqrt(x*x + y*y) computed with scaling behavior suitable for floating-point range.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/hypot
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T hypot(T x, T y) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_hypot<T>) { return ccm::builtin::hypot(x, y); }
		else if (ccm::support::is_constant_evaluated())
		{
			if constexpr (std::is_same_v<T, float>) { return internal::impl::hypot_impl(x, y); }
			else if constexpr (std::is_same_v<T, double>) { return internal::impl::hypot_impl(x, y); }
			else
			{
				return static_cast<long double>(internal::impl::hypot_impl(static_cast<double>(x), static_cast<double>(y)));
			}
		}
		else
		{
			return ccm::rt::hypot_rt(x, y);
		}
	}

	/**
	 * @brief Computes the length of the three-dimensional hypotenuse.
	 * @tparam T Floating-point type.
	 * @param x First value.
	 * @param y Second value.
	 * @param z Third value.
	 * @return sqrt(x*x + y*y + z*z) computed with the same scaling behavior as the 2-argument overload.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/hypot
	 */
	constexpr float hypot(float x, float y, float z) noexcept
	{ return ccm::hypot(ccm::hypot(x, y), z); }

	constexpr double hypot(double x, double y, double z) noexcept
	{ return ccm::hypot(ccm::hypot(x, y), z); }

	constexpr long double hypot(long double x, long double y, long double z) noexcept
	{ return ccm::hypot(ccm::hypot(x, y), z); }

	template <typename Arithmetic1,
			  typename Arithmetic2,
			  typename Arithmetic3,
			  std::enable_if_t<std::is_arithmetic_v<Arithmetic1> && std::is_arithmetic_v<Arithmetic2> && std::is_arithmetic_v<Arithmetic3>, bool> = true>
	constexpr auto hypot(Arithmetic1 x, Arithmetic2 y, Arithmetic3 z) noexcept
	{
		using shared_type = std::common_type_t<Arithmetic1, Arithmetic2, Arithmetic3>;
		return ccm::hypot(static_cast<shared_type>(x), static_cast<shared_type>(y), static_cast<shared_type>(z));
	}

	/**
	 * @brief Computes the hypotenuse for float values.
	 * @param x First value.
	 * @param y Second value.
	 * @return Hypotenuse length as float.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/hypot
	 */
	constexpr float hypotf(float x, float y) noexcept
	{ return ccm::hypot(x, y); }

	/**
	 * @brief Computes the hypotenuse for long double values.
	 * @param x First value.
	 * @param y Second value.
	 * @return Hypotenuse length as long double.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/hypot
	 */
	constexpr long double hypotl(long double x, long double y) noexcept
	{ return ccm::hypot(x, y); }
} // namespace ccm

/// @ingroup power
