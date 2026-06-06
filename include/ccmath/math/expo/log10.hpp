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
#include "ccmath/internal/math/generic/builtins/expo/log10.hpp"
#include "ccmath/internal/math/runtime/func/expo/log10_rt.hpp"
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fp/directional_rounding_utils.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/math/expo/impl/log10_impl.hpp"

#include <limits>
#include <type_traits>

#if defined(_MSC_VER) && !defined(__clang__)
	#include "ccmath/internal/predef/compiler_suppression/msvc_compiler_suppression.hpp"
CCM_DISABLE_MSVC_WARNING(4702)
#endif

namespace ccm
{
	/**
	 * @brief Computes the base-10 logarithm of a floating-point value.
	 * @tparam T Floating-point type.
	 * @param num Floating-point value.
	 * @return Base-10 logarithm of num; returns -inf for zero and NaN for negative inputs.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/log10
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T log10(T num)
	{
		if constexpr (ccm::builtin::has_constexpr_log10<T>)
		{
			if (ccm::support::is_constant_evaluated()) { return ccm::builtin::log10(num); }
		}
		{
			if (num == static_cast<T>(1))
			{
				if (ccm::support::is_constant_evaluated()) { return static_cast<T>(0); }
#if defined(CCMATH_COMPILER_APPLE_CLANG) || (defined(_MSC_VER) && !defined(__clang__))
				return ccm::support::fp::signed_zero_for_current_mode<T>();
#else
				return static_cast<T>(0);
#endif
			}
			if (num == static_cast<T>(0))
			{
				ccm::support::fenv::set_errno_if_required(ERANGE);
				ccm::support::fenv::raise_except_if_required(FE_DIVBYZERO);
				return -std::numeric_limits<T>::infinity();
			}
			if (num < static_cast<T>(0))
			{
				ccm::support::fenv::set_errno_if_required(EDOM);
				ccm::support::fenv::raise_except_if_required(FE_INVALID);
				return -std::numeric_limits<T>::quiet_NaN();
			}
			if (CCM_UNLIKELY(num == std::numeric_limits<T>::infinity())) { return std::numeric_limits<T>::infinity(); }
			if (CCM_UNLIKELY(ccm::isnan(num))) { return std::numeric_limits<T>::quiet_NaN(); }

			if (!ccm::support::is_constant_evaluated()) { return ccm::rt::log10_rt(num); }

			if constexpr (std::is_same_v<T, float>) { return internal::log10_float(num); }
			if constexpr (std::is_same_v<T, double>) { return internal::log10_double(num); }
			if constexpr (std::is_same_v<T, long double>) { return static_cast<long double>(internal::log10_double(static_cast<double>(num))); }
			return static_cast<T>(internal::log10_double(static_cast<double>(num)));
		}
	}

	/**
	 * @brief Computes base-10 logarithm after promoting an integer input to double.
	 * @tparam Integer Integral type.
	 * @param num Integer value.
	 * @return Base-10 logarithm of num as double.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/log10
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double log10(Integer num)
	{ return ccm::log10<double>(static_cast<double>(num)); }

	/**
	 * @brief Computes the base-10 logarithm of a float.
	 * @param num Floating-point value.
	 * @return Base-10 logarithm as float.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/log10
	 */
	constexpr float log10f(float num)
	{ return ccm::log10<float>(num); }

	/**
	 * @brief Computes the base-10 logarithm of a long double.
	 * @param num Floating-point value.
	 * @return Base-10 logarithm as long double.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/log10
	 */
	constexpr long double log10l(long double num)
	{ return ccm::log10<long double>(num); }
} // namespace ccm
