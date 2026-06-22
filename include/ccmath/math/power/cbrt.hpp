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

#include "ccmath/internal/math/generic/builtins/power/cbrt.hpp"
#include "ccmath/internal/math/generic/func/power/cbrt_gen.hpp"
#include "ccmath/internal/math/runtime/func/power/cbrt_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Computes the cube root of a floating-point value.
	 * @tparam T Floating-point type.
	 * @param num Floating-point value.
	 * @return Cube root of num.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T cbrt(T num) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_cbrt<T>) { return ccm::builtin::cbrt_ct(num); }
		else
		{
			if (ccm::support::is_constant_evaluated()) { return ccm::gen::cbrt_gen<T>(num); }
			return ccm::rt::cbrt_rt<T>(num);
		}
	}

	/**
	 * @brief Computes the cube root of a float.
	 * @param num Floating-point value.
	 * @return Cube root of num as float.
	 */
	constexpr float cbrtf(float num) noexcept
	{ return ccm::cbrt(num); }

	/**
	 * @brief Computes the cube root of a long double.
	 * @param num Floating-point value.
	 * @return Cube root of num as long double.
	 */
	constexpr long double cbrtl(long double num) noexcept
	{ return ccm::cbrt(num); }
} // namespace ccm

/// @ingroup power
