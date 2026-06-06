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

#include "ccmath/internal/config/builtin/exp2_support.hpp"
#include "ccmath/internal/math/generic/builtins/expo/exp2.hpp"
#include "ccmath/internal/predef/has_const_builtin.hpp"
#include "ccmath/math/expo/impl/exp2_double_impl.hpp"
#include "ccmath/math/expo/impl/exp2_float_impl.hpp"

#include <type_traits>

#if defined(_MSC_VER) && !defined(__clang__)
	#include "ccmath/internal/predef/compiler_suppression/msvc_compiler_suppression.hpp"
CCM_DISABLE_MSVC_WARNING(4702) // 4702: unreachable code
#endif

namespace ccm
{
	/**
	 * @brief Returns 2 raised to the given power (2^x)
	 * @tparam T Floating-point or integer type
	 * @param num Floating-point or integer value
	 * @return If no errors occur, the base-2 exponential of num (2^num) is returned.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T exp2(T num)
	{
		if constexpr (ccm::builtin::has_constexpr_exp2<T>) { return ccm::builtin::exp2(num); }
		else
		{
			if constexpr (std::is_same_v<T, float>) { return internal::exp2_float(num); }
			if constexpr (std::is_same_v<T, double>) { return internal::exp2_double(num); }
			if constexpr (std::is_same_v<T, long double>) { return static_cast<long double>(internal::exp2_double(static_cast<double>(num))); }
			return static_cast<T>(internal::exp2_double(static_cast<double>(num)));
		}
	}

	/**
	 * @brief Returns 2 raised to the given power (2^x)
	 * @tparam Integer Integer type
	 * @param num Integer value
	 * @return If no errors occur, the base-2 exponential of num (2^num) is returned as a double.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double exp2(Integer num)
	{
		return ccm::exp2<double>(static_cast<double>(num));
	}

	/**
	 * @brief Returns 2 raised to the given power (2^x)
	 * @param num Floating-point value
	 * @return If no errors occur, the base-2 exponential of num (2^num) is returned as a float.
	 */
	constexpr float exp2f(float num)
	{
		return ccm::exp2<float>(num);
	}

	/**
	 * @brief Returns 2 raised to the given power (2^x)
	 * @param num Floating-point value
	 * @return If no errors occur, the base-2 exponential of num (2^num) is returned as a double.
	 */
	constexpr long double exp2l(long double num)
	{
		return ccm::exp2<long double>(num);
	}
} // namespace ccm

#if defined(_MSC_VER) && !defined(__clang__)
CCM_RESTORE_MSVC_WARNING()
#endif
