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

#include "ccmath/internal/math/generic/builtins/misc/tgamma.hpp"
#include "ccmath/internal/math/runtime/func/misc/gamma_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/misc/impl/gamma_impl.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Computes the gamma function.
	 * @tparam T floating-point or integer type
	 * @param num floating-point or integer value
	 * @return If no errors occur, the gamma function value of num is returned.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T gamma(T num)
	{
		if constexpr (ccm::builtin::has_constexpr_gamma<T>) { return ccm::builtin::gamma(num); }
		else
		{
			if (!ccm::support::is_constant_evaluated()) { return ccm::rt::gamma_rt(num); }

			if constexpr (std::is_same_v<T, float>) { return internal::gamma_float(num); }
			if constexpr (std::is_same_v<T, double>) { return internal::gamma_double(num); }
			if constexpr (std::is_same_v<T, long double>) { return static_cast<long double>(internal::gamma_double(static_cast<double>(num))); }
			return static_cast<T>(internal::gamma_double(static_cast<double>(num)));
		}
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double gamma(Integer num)
	{
		return ccm::gamma<double>(static_cast<double>(num));
	}

	constexpr float tgammaf(float num)
	{
		return ccm::gamma<float>(num);
	}

	constexpr long double tgammal(long double num)
	{
		return ccm::gamma<long double>(num);
	}
} // namespace ccm
