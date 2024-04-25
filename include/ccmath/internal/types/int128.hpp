/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once
#include <cstdint>

namespace ccm
{
	// __SIZEOF_INT128__ is generally defined when the compiler supports 128-bit integers.
#ifdef __SIZEOF_INT128__
	using int128_t = __int128;
	using uint128_t = unsigned __int128;
#else
	// Currently this is assuming we are handling MSVC here.
	// Until we have implemented big_ints, we will use int64_t.
	using int128_t = std::int64_t;
	using uint128_t = std::uint64_t;
#endif
} // namespace ccm