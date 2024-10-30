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

namespace ccm::support::helpers
{
	constexpr int digit_to_int(int c)
	{
		if ('0' <= c && c <= '9')
		{
			return (c - '0');
		}
		if ('A' <= c && c <= 'F')
		{
			return (c - 'A' + 10);
		}
		if ('a' <= c && c <= 'f')
		{
			return (c - 'a' + 10);
		}
		return 0;
	}
} // namespace ccm::support::helpers
