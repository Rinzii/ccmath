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

#include "ccmath/internal/math/generic/builtins/expo/log1p.hpp"
#include "ccmath/internal/math/generic/func/expo/log1p_gen.hpp"
#include "ccmath/internal/math/runtime/func/expo/log1p_rt.hpp"
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/math/expo/impl/log1p_impl.hpp"

#include <limits>
#include <type_traits>

#if defined(_MSC_VER) && !defined(__clang__)
	#include "ccmath/internal/predef/compiler_suppression/msvc_compiler_suppression.hpp"
CCM_DISABLE_MSVC_WARNING(4702)
#endif

namespace ccm
{
	/**
	 * @brief Computes log(1 + num) with improved accuracy near zero.
	 * @tparam T Floating-point type.
	 * @param num Floating-point value.
	 * @return Natural logarithm of 1 + num. Returns -inf for num == -1 and NaN for num < -1.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/log1p
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T log1p(T num)
	{
		if constexpr (ccm::builtin::has_constexpr_log1p<T>) { return ccm::builtin::log1p_ct(num); }
		else
		{
			if (num == static_cast<T>(0))
			{
				if (!ccm::support::is_constant_evaluated()) { return ccm::rt::log1p_rt(num); }
				return internal::log1p_zero_result(num);
			}

			if (CCM_UNLIKELY(ccm::isnan(num))) { return std::numeric_limits<T>::quiet_NaN(); }

			if (num == std::numeric_limits<T>::infinity()) { return std::numeric_limits<T>::infinity(); }

			if (num == static_cast<T>(-1))
			{
				ccm::support::fenv::set_errno_if_required(ERANGE);
				ccm::support::fenv::raise_except_if_required(FE_DIVBYZERO);
				return -std::numeric_limits<T>::infinity();
			}

			if (num < static_cast<T>(-1))
			{
				ccm::support::fenv::set_errno_if_required(EDOM);
				ccm::support::fenv::raise_except_if_required(FE_INVALID);
				return -std::numeric_limits<T>::quiet_NaN();
			}

			if (ccm::support::is_constant_evaluated()) { return gen::log1p_gen(num); }

			return ccm::rt::log1p_rt(num);
		}
	}

	/**
	 * @brief Computes log(1 + num) after promoting an integer input to double.
	 * @tparam Integer Integral type.
	 * @param num Integer value.
	 * @return Natural logarithm of 1 + num as double.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/log1p
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double log1p(Integer num)
	{ return ccm::log1p<double>(static_cast<double>(num)); }

	/**
	 * @brief Computes log(1 + num) for float.
	 * @param num Floating-point value.
	 * @return Natural logarithm of 1 + num as float.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/log1p
	 */
	constexpr float log1pf(float num)
	{ return ccm::log1p<float>(num); }

	/**
	 * @brief Computes log(1 + num) for long double.
	 * @param num Floating-point value.
	 * @return Natural logarithm of 1 + num as long double.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/log1p
	 */
	constexpr long double log1pl(long double num)
	{ return ccm::log1p<long double>(num); }
} // namespace ccm
