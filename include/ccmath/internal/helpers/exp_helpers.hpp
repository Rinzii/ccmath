/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/meta_compare.hpp"
#include <cfloat>
#include <type_traits>

namespace ccm::helpers
{
#if FLT_EVAL_METHOD == 1
	// check if the type is the same as float
	template <typename T>
	inline constexpr bool check_excess_precision = std::is_same_v<T, float>;
#else
	// check if the type is the same as float or double
	template <typename T>
	inline constexpr bool check_excess_precision = ccm::support::or_v<std::is_same<T, float>, std::is_same<T, double>>;
#endif

	template <typename T>
	constexpr T narrow_eval(T x)
	{
#if FLT_EVAL_METHOD == 0
		return x;
#else
		T math_narrow_eval_tmp = x;
		if constexpr (check_excess_precision<T>)
		{
			volatile T volatile_tmp = ccm::support::bit_cast<volatile T>(math_narrow_eval_tmp);
			math_narrow_eval_tmp	= volatile_tmp;
		}
		return math_narrow_eval_tmp;
#endif
	}
} // namespace ccm::helpers
