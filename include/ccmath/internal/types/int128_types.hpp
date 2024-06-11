/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/type_support.hpp"

#ifndef CCM_TYPES_HAS_INT128
	#include "ccmath/internal/types/big_int.hpp"
#endif

namespace ccm::types
{
#ifdef CCM_TYPES_HAS_INT128
	using int128_t	= __int128_t;
	using uint128_t = __uint128_t;
#else
	using int128_t	= Int<128>;
	using uint128_t = UInt<128>;
#endif

} // namespace ccm::types
