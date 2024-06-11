/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

namespace ccm::helpers
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
} // namespace ccm::helpers
