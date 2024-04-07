/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <cstdint>

namespace ccm::helpers
{
	enum class endian : std::uint16_t
	{
		// TODO: Add support for other compilers besides just MSVC, GCC, and Clang.

#if defined(_MSC_VER) && !defined(__clang__)
		little = 0,
		big    = 1,
		native = little
#else
		little = __ORDER_LITTLE_ENDIAN__,
		big    = __ORDER_BIG_ENDIAN__,
		native = __BYTE_ORDER__
#endif
	};
} // namespace ccm::helpers


