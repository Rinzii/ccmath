/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

namespace ccm::support::traits
{
	template <typename T, T v>
	struct integral_constant
	{
		using value_type		 = T;
		static constexpr T value = v;
	};
} // namespace ccm::support::traits