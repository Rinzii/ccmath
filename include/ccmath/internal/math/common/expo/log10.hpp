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

#include "ccmath/internal/math/generic/builtins/expo/log10.hpp"
#include "ccmath/internal/math/generic/func/expo/log10_gen.hpp"
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/math/compare/isnan.hpp"

#include <limits>
#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T log10(T num)
	{
		if constexpr (ccm::builtin::has_constexpr_log10<T>) { return ccm::builtin::log10(num); }
		else
		{
			if (num == static_cast<T>(1)) { return static_cast<T>(0); }
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

			return gen::log10_gen(num);
		}
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double log10(Integer num)
	{ return ccm::log10<double>(static_cast<double>(num)); }

	constexpr float log10f(float num)
	{ return ccm::log10<float>(num); }

	constexpr long double log10l(long double num)
	{ return ccm::log10<long double>(num); }
} // namespace ccm
