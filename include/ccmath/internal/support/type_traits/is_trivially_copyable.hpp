/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/type_traits/integral_constant.hpp"

namespace ccm::support::traits
{
	template <class T>
	struct is_trivially_copyable : integral_constant<bool, __is_trivially_copyable(T)>
	{
	};

	template <class T>
	inline constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;

} // namespace ccm::support::traits