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
	template <bool V> using bool_constant = ccm::support::traits::integral_constant<bool, V>;
} // namespace ccm::support::traits