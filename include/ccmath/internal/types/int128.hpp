/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/type_support.hpp"
#include "ccmath/internal/types/big_int.hpp"

#include <cstdint>

namespace ccm
{
	// __SIZEOF_INT128__ is generally defined when the compiler supports 128-bit integers.
#ifdef CCM_TYPES_HAS_INT128
	using Int128 = __int128;
	using Uint128 = unsigned __int128;
#else
	// Currently this is assuming we are handling MSVC here.
	// Until we have implemented big_ints, we will use int64_t.
	using Int128 = ccm::support::Int<128>;
	using Uint128 = ccm::support::UInt<128>;
#endif
} // namespace ccm