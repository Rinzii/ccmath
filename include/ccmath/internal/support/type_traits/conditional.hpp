/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/type_traits/type_identity.hpp"

namespace ccm::support::traits
{
	template <bool B, typename T, typename F>
	struct conditional : type_identity<T>
	{
	};
	template <typename T, typename F>
	struct conditional<false, T, F> : type_identity<F>
	{
	};
	template <bool B, typename T, typename F>
	using conditional_t = typename conditional<B, T, F>::type;
} // namespace ccm::support::traits