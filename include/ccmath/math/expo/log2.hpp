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

#include "ccmath/internal/config/compiler.hpp"
#include "ccmath/internal/math/generic/builtins/expo/log2.hpp"
#include "ccmath/internal/math/generic/func/expo/log2_gen.hpp"
#include "ccmath/internal/math/runtime/func/expo/log2_rt.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fp/directional_rounding_utils.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/math/compare/signbit.hpp"

#include <limits>
#include <type_traits>

#if defined(_MSC_VER) && !defined(__clang__)
	#include "ccmath/internal/predef/compiler_suppression/msvc_compiler_suppression.hpp"
CCM_DISABLE_MSVC_WARNING(4702)
#endif

namespace ccm
{
	/**
	 * @brief Returns the base 2 logarithm of a number.
	 * @tparam T The type of the number.
	 * @param num The number to calculate the base 2 logarithm of.
	 * @return The base 2 logarithm of the number.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/log2
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T log2(T num) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_log2<T>)
		{
			if (ccm::support::is_constant_evaluated()) { return ccm::builtin::log2(num); }
		}
		{
			// If the argument is ±0, -∞ is returned
			if (num == static_cast<T>(0))
			{
				ccm::support::fenv::set_errno_if_required(ERANGE);
				ccm::support::fenv::raise_except_if_required(FE_DIVBYZERO);
				return -std::numeric_limits<T>::infinity();
			}

			// If the argument is 1, exact zero is returned.
			if (num == static_cast<T>(1))
			{
				if (ccm::support::is_constant_evaluated()) { return static_cast<T>(0); }
#if defined(CCMATH_COMPILER_APPLE_CLANG)
				return ccm::support::fp::signed_zero_for_current_mode<T>();
#elif defined(_MSC_VER) && !defined(__clang__)
				if constexpr (std::is_same_v<T, float>) { return static_cast<T>(0); }
				else
				{
					return ccm::support::fp::signed_zero_for_current_mode<T>();
				}
#else
				return static_cast<T>(0);
#endif
			}

			// If the argument is NaN, NaN is returned.
			if (ccm::isnan(num) || num == std::numeric_limits<T>::infinity()) { return num; }

// If the argument is negative, -NaN is returned
#ifdef CCMATH_COMPILER_APPLE_CLANG // Apple clang returns +qNaN
			if (ccm::signbit(num))
			{
				ccm::support::fenv::set_errno_if_required(EDOM);
				ccm::support::fenv::raise_except_if_required(FE_INVALID);
				return std::numeric_limits<T>::quiet_NaN();
			}
#else // All other major compilers return -qNaN
			if (ccm::signbit(num))
			{
				ccm::support::fenv::set_errno_if_required(EDOM);
				ccm::support::fenv::raise_except_if_required(FE_INVALID);
				return -std::numeric_limits<T>::quiet_NaN();
			}
#endif

			if (!ccm::support::is_constant_evaluated()) { return ccm::rt::log2_rt(num); }

			return gen::log2_gen(num);
		}
	}

	/**
	 * @brief Returns the base 2 logarithm of a number.
	 * @tparam Integer The type of the integer.
	 * @param num The number to calculate the base 2 logarithm of.
	 * @return The base 2 logarithm of the number as a double.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/log2
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double log2(Integer num) noexcept
	{ return ccm::log2<double>(static_cast<double>(num)); }

	/**
	 * @brief Returns the base 2 logarithm of a number.
	 * @param num The number to calculate the base 2 logarithm of.
	 * @return The base 2 logarithm of the number as a float.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/log2
	 */
	constexpr float log2f(float num)
	{ return ccm::log2<float>(num); }

	/**
	 * @brief Returns the base 2 logarithm of a number.
	 * @param num The number to calculate the base 2 logarithm of.
	 * @return The base 2 logarithm of the number as a double.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/log2
	 */
	constexpr long double log2l(long double num)
	{ return ccm::log2<long double>(num); }
} // namespace ccm

#if defined(_MSC_VER) && !defined(__clang__)
CCM_RESTORE_MSVC_WARNING()
#endif
