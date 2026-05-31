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

#include "ccmath/internal/math/generic/builtins/trig/tan.hpp"
#include "ccmath/internal/math/generic/func/trig/tan_gen.hpp"
#include "ccmath/internal/math/runtime/func/trig/tan_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"

#include <type_traits>

namespace ccm
{
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T tan(T num)
	{
		if constexpr (ccm::builtin::has_constexpr_tan<T>) { return ccm::builtin::tan(num); }
		else
		{
			if (ccm::support::is_constant_evaluated()) { return ccm::gen::tan_gen(num); }
			return ccm::rt::tan_rt(num);
		}
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double tan(Integer num)
	{
		return ccm::tan<double>(static_cast<double>(num));
	}

	constexpr float tanf(float num)
	{
		return ccm::tan<float>(num);
	}

	constexpr long double tanl(long double num)
	{
		return ccm::tan<long double>(num);
	}
} // namespace ccm

/// @ingroup trig
