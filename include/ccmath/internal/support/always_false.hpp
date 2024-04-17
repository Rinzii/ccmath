/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

namespace ccm::support
{
	// Explanation of the purpose of this template.
	// https://artificial-mind.net/blog/2020/10/03/always-false
	template <typename...>
	inline constexpr bool always_false = false;
} // namespace ccm::support
