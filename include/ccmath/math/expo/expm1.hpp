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

#include "ccmath/internal/math/generic/builtins/expo/expm1.hpp"
#include "ccmath/internal/math/generic/func/expo/expm1_gen.hpp"
#include "ccmath/internal/math/runtime/func/expo/expm1_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"

#include <type_traits>

#if defined(_MSC_VER) && !defined(__clang__)
	#include "ccmath/internal/predef/compiler_suppression/msvc_compiler_suppression.hpp"
CCM_DISABLE_MSVC_WARNING(4702)
#endif

namespace ccm
{
	/**
	 * @brief Computes exp(num) - 1 with improved accuracy near zero.
	 * @tparam T Floating-point type.
	 * @param num Floating-point value.
	 * @return exp(num) - 1.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/expm1
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T expm1(T num)
	{
		if constexpr (ccm::builtin::has_constexpr_expm1<T>) { return ccm::builtin::expm1(num); }
		else
		{
			if (!ccm::support::is_constant_evaluated()) { return ccm::rt::expm1_rt(num); }

			return gen::expm1_gen(num);
		}
	}

	/**
	 * @brief Computes exp(num) - 1 after promoting an integer input to double.
	 * @tparam Integer Integral type.
	 * @param num Integer value.
	 * @return exp(num) - 1 as double.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/expm1
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double expm1(Integer num)
	{
		return ccm::expm1<double>(static_cast<double>(num));
	}

	/**
	 * @brief Computes exp(num) - 1 for float.
	 * @param num Floating-point value.
	 * @return exp(num) - 1 as float.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/expm1
	 */
	constexpr float expm1f(float num)
	{
		return ccm::expm1<float>(num);
	}

	/**
	 * @brief Computes exp(num) - 1 for long double.
	 * @param num Floating-point value.
	 * @return exp(num) - 1 as long double.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/expm1
	 */
	constexpr long double expm1l(long double num)
	{
		return ccm::expm1<long double>(num);
	}
} // namespace ccm
