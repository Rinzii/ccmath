/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/type_support.hpp"

#include <cstdint>

// Delete this when we have implemented uint128_t and int128_t.
struct ccm_int128_t
{
	std::int64_t low;
	std::int64_t high;
};

struct ccm_uint128_t
{
	std::uint64_t low;
	std::uint64_t high;
};

namespace ccm
{
	// __SIZEOF_INT128__ is generally defined when the compiler supports 128-bit integers.
#ifdef CCM_TYPES_HAS_INT128
	using int128_t = __int128;
	using uint128_t = unsigned __int128;
#else
	// Currently this is assuming we are handling MSVC here.
	// Until we have implemented big_ints, we will use int64_t.
	using int128_t = ccm_uint128_t
	using uint128_t = ccm_int128_t
#endif
} // namespace ccm