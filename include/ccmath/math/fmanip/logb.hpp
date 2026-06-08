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

#include "ccmath/internal/math/generic/builtins/fmanip/logb.hpp"
#include "ccmath/internal/math/generic/func/fmanip/logb_gen.hpp"
#include "ccmath/internal/math/runtime/func/fmanip/logb_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Extracts the unbiased exponent of a floating-point value as a floating-point result.
	 * @tparam T Floating-point type.
	 * @param num Floating-point value.
	 * @return Unbiased exponent of num in the return type T.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/logb
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T logb(T num) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_logb<T>) { return ccm::builtin::logb(num); }
		else
		{
			if (ccm::support::is_constant_evaluated()) { return ccm::gen::logb_gen(num); }
			return ccm::rt::logb_rt(num);
		}
	}

	/**
	 * @brief Extracts the unbiased exponent of a float.
	 * @param num Floating-point value.
	 * @return Unbiased exponent as float.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/logb
	 */
	constexpr float logbf(float num) noexcept
	{ return ccm::logb(num); }

	/**
	 * @brief Extracts the unbiased exponent of a long double.
	 * @param num Floating-point value.
	 * @return Unbiased exponent as long double.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/logb
	 */
	constexpr long double logbl(long double num) noexcept
	{ return ccm::logb(num); }
} // namespace ccm
