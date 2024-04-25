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
	// Not required in GCC 13.1+ and Clang 17.0.1+ but applied anyway for wider compatibility.
	// See: https://cplusplus.github.io/CWG/issues/2518.html
	template <typename...>
	inline constexpr bool always_false = false;
} // namespace ccm::support
