/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/type_traits/false_type.hpp"
#include "ccmath/internal/support/type_traits/true_type.hpp"

namespace ccm::support::traits
{
	template <typename T, typename U>
	struct is_same : false_type {};

	template <typename T>
	struct is_same<T, T> : true_type {};

	template <typename T, typename U>
	constexpr bool is_same_v = is_same<T, U>::value;
} // namespace ccm::support::traits