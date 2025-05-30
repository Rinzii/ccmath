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

#include "ccmath/internal/math/generic/builtins/compare/islessgreater.hpp"
#include "ccmath/math/compare/isunordered.hpp"

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Checks if the first argument is less than the second or greater than the second.
	 * @tparam T Type of the values to compare.
	 * @param x A floating-point or integer value.
	 * @param y A floating-point or integer value.
	 * @return true if the first argument is less than the second or greater than the second, false otherwise.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr bool islessgreater(T x, T y) noexcept
	{
		if constexpr (ccm::builtin::has_constexpr_islessgreater<T>) { return ccm::builtin::islessgreater(x, y); }
		else { return !ccm::isunordered(x, y) && (x < y || x > y); }
	}

	/**
	 * @brief Checks if the first argument is less than the second or greater than the second.
	 * @tparam Arithmetic1 Arithmetic type of the left-hand side.
	 * @tparam Arithmetic2 Arithmetic type of the right-hand side.
	 * @param x Value of the left-hand side of the comparison.
	 * @param y Value of the right-hand side of the comparison.
	 * @return true if the first argument is less than the second or greater than the second, false otherwise.
	 */
	template <typename Arithmetic1, typename Arithmetic2, std::enable_if_t<std::is_arithmetic_v<Arithmetic1> && std::is_arithmetic_v<Arithmetic2>, bool> = true>
	constexpr bool islessgreater(Arithmetic1 x, Arithmetic2 y) noexcept
	{
		using shared_type = std::common_type_t<Arithmetic1, Arithmetic2>;
		return static_cast<shared_type>(islessgreater<shared_type>(static_cast<shared_type>(x), static_cast<shared_type>(y)));
	}
} // namespace ccm

/// @ingroup compare
