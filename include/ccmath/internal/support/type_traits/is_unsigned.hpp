/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/type_traits/bool_constant.hpp"
#include "ccmath/internal/support/type_traits/is_arithmetic.hpp"

namespace ccm::support::traits
{
	template <typename T>
	struct is_unsigned : bool_constant<(is_arithmetic_v<T> && (T(-1) > T(0)))>
	{
		constexpr operator bool() const { return is_unsigned::value; }
		constexpr bool operator()() const { return is_unsigned::value; }
	};
	template <typename T>
	constexpr bool is_unsigned_v = is_unsigned<T>::value;

} // namespace ccm::support::traits