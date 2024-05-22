/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/type_traits/bool_constant.hpp"
#include "ccmath/internal/support/type_traits/is_floating_point.hpp"
#include "ccmath/internal/support/type_traits/is_integral.hpp"

namespace ccm::support::traits
{
	template <typename T>
	struct is_arithmetic : bool_constant<(is_integral_v<T> || is_floating_point_v<T>)>
	{
	};
	template <typename T>
	constexpr bool is_arithmetic_v = is_arithmetic<T>::value;

} // namespace ccm::support::traits