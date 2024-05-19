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
	template <class T, class... Args>
	struct is_trivially_constructible : integral_constant<bool, __is_trivially_constructible(T, Args...)>
	{
	};

	template <class T, class... Args>
	inline constexpr bool is_trivially_constructible_v = is_trivially_constructible<T, Args...>::value;

} // namespace ccm::support::traits