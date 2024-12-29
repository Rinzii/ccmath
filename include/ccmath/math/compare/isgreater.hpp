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

#include <type_traits>

#include "ccmath/internal/math/generic/builtins/compare/isgreater.hpp"


namespace ccm
{
	/**
	 * @brief Checks if the first argument is greater than the second.
	 * @tparam T Type of the values to compare.
	 * @param x A floating-point or integer value.
	 * @param y A floating-point or integer value.
	 * @return true if the first argument is greater than the second, false otherwise.
	 */
	template <typename T>
	constexpr bool isgreater(T x, T y) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_isgreater<T>) { return ccm::builtin::isgreater(x, y); }
		else
		{
			return x > y;
		}
	}

	/**
	 * @brief Checks if the first argument is greater than the second.
	 * @tparam T Type of the left-hand side.
	 * @tparam U Type of the right-hand side.
	 * @param x Value of the left-hand side of the comparison.
	 * @param y Value of the right-hand side of the comparison.
	 * @return true if the first argument is greater than the second, false otherwise.
	 */
	template <typename T, typename U>
	constexpr bool isgreater(T x, U y) noexcept
	{
		// Find the common type of the two arguments
		using shared_type = std::common_type_t<T, U>;

		// Then cast the arguments to the common type and call the single argument version
		return static_cast<shared_type>(isgreater<shared_type>(static_cast<shared_type>(x), static_cast<shared_type>(y)));
	}
} // namespace ccm

/// @ingroup compare
