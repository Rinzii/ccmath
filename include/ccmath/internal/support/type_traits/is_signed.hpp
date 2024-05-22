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
	struct is_signed : bool_constant<(is_arithmetic_v<T> && (T(-1) < T(0)))>
	{
		constexpr operator bool() const { return is_signed::value; }
		constexpr bool operator()() const { return is_signed::value; }
	};
	template <typename T>
	constexpr bool is_signed_v = is_signed<T>::value;

} // namespace ccm::support::traits