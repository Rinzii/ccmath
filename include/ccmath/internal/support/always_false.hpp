/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
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
