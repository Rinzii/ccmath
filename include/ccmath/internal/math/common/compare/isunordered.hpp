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

#include "ccmath/internal/math/generic/builtins/compare/isunordered.hpp"
#include "ccmath/math/compare/isnan.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Checks if the given number is unordered, that is, one or both are NaN and thus cannot be meaningfully compared with each other.
	 * @tparam T The type of the number.
	 * @param x A floating-point or integer value
	 * @param y A floating-point or integer value
	 * @return true if either x or y is NaN, false otherwise.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr bool isunordered(T x, T y) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_isunordered<T>) { return ccm::builtin::isunordered(x, y); }
		else { return ccm::isnan(x) || ccm::isnan(y); }
	}

	/**
	 * @brief Checks if the given number is unordered, that is, one or both are NaN and thus cannot be meaningfully compared with each other.
	 * @tparam Integer The type of the integer.
	 * @return false, as all integers are ordered.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr bool isunordered(Integer /* x */, Integer /* y */) noexcept
	{
		return false; // All integers are ordered
	}

	/**
	 * @brief Checks if the given number is unordered, that is, one or both are NaN and thus cannot be meaningfully compared with each other.
	 * @tparam T Type of the left-hand side.
	 * @tparam U Type of the right-hand side.
	 * @param x Value of the left-hand side of the comparison.
	 * @param y Value of the right-hand side of the comparison.
	 * @return true if either x or y is NaN, false otherwise.
	 */
	template <typename T, typename U>
	constexpr bool isunordered(T x, U y) noexcept
	{
		using shared_type = std::common_type_t<T, U>;
		return static_cast<shared_type>(isunordered<shared_type>(static_cast<shared_type>(x), static_cast<shared_type>(y)));
	}
} // namespace ccm

/// @ingroup compare
