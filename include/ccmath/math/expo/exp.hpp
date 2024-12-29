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

#include "ccmath/math/expo/impl/exp_double_impl.hpp"
#include "ccmath/math/expo/impl/exp_float_impl.hpp"
#include "ccmath/internal/math/generic/builtins/expo/exp.hpp"


#if defined(_MSC_VER) && !defined(__clang__)
#include "ccmath/internal/predef/compiler_suppression/msvc_compiler_suppression.hpp"
CCM_DISABLE_MSVC_WARNING(4702) // 4702: unreachable code
#endif

namespace ccm
{
	/**
	 * @brief Computes e raised to the given power
	 * @tparam T floating-point or integer type
	 * @param num floating-point or integer value
	 * @return If no errors occur, the base-e exponential of num (e^num) is returned.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T exp(T num)
	{
		if constexpr (ccm::builtin::has_constexpr_exp<T>) { return ccm::builtin::exp(num); }
		else
		{
			if constexpr (std::is_same_v<T, float>) { return internal::impl::exp_float_impl(num); }
			if constexpr (std::is_same_v<T, double>) { return internal::impl::exp_double_impl(num); }
			if constexpr (std::is_same_v<T, long double>) { return static_cast<long double>(internal::impl::exp_double_impl(static_cast<double>(num))); }
			return static_cast<T>(internal::impl::exp_double_impl(static_cast<double>(num)));
		}
	}

	/**
	 * @brief Computes e raised to the given power
	 * @tparam Integer integer type
	 * @param num integer value
	 * @return If no errors occur, the base-e exponential of num (e^num) is returned as double.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double exp(Integer num)
	{
		return ccm::exp<double>(static_cast<double>(num));
	}

	/**
	 * @brief Computes e raised to the given power
	 * @param num floating-point value
	 * @return If no errors occur, the base-e exponential of num (e^num) is returned as float.
	 */
	constexpr float expf(float num)
	{
		return ccm::exp<float>(num);
	}

	/**
	 * @brief Computes e raised to the given power
	 * @param num floating-point value
	 * @return If no errors occur, the base-e exponential of num (e^num) is returned as double.
	 */
	constexpr long double expl(long double num)
	{
		return ccm::exp<long double>(num);
	}

} // namespace ccm

#if defined(_MSC_VER) && !defined(__clang__)
CCM_RESTORE_MSVC_WARNING()
#endif
