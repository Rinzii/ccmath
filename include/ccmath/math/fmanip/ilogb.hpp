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

#include "ccmath/internal/math/generic/builtins/fmanip/ilogb.hpp"
#include "ccmath/internal/math/runtime/func/fmanip/ilogb_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/math/fmanip/impl/ilogb_impl.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Extracts the unbiased integer exponent of a floating-point value.
	 * @tparam T Floating-point type.
	 * @param num Floating-point value.
	 * @return Unbiased base-2 exponent as int (or implementation-defined special values for zero/NaN).
	 * @see https://en.cppreference.com/w/cpp/numeric/math/ilogb
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr int ilogb(T num) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_ilogb<T>) { return ccm::builtin::ilogb(num); }
		else if (ccm::support::is_constant_evaluated())
		{
			if constexpr (std::is_same_v<T, float>) { return internal::impl::ilogb_impl(num); }
			else if constexpr (std::is_same_v<T, double>) { return internal::impl::ilogb_impl(num); }
			else
			{
				return internal::impl::ilogb_impl(static_cast<double>(num));
			}
		}
		else
		{
			return ccm::rt::ilogb_rt(num);
		}
	}

	/**
	 * @brief Extracts the unbiased integer exponent of a float.
	 * @param num Floating-point value.
	 * @return Unbiased base-2 exponent as int.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/ilogb
	 */
	constexpr int ilogbf(float num) noexcept
	{ return ccm::ilogb(num); }

	/**
	 * @brief Extracts the unbiased integer exponent of a long double.
	 * @param num Floating-point value.
	 * @return Unbiased base-2 exponent as int.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/ilogb
	 */
	constexpr int ilogbl(long double num) noexcept
	{ return ccm::ilogb(num); }
} // namespace ccm
