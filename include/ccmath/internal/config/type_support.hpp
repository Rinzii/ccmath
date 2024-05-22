/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once


#include <cfloat> // LDBL_MANT_DIG
#include <cstdint> // UINT64_MAX, __SIZEOF_INT128__

// int128 / uint128 support
#if defined(__SIZEOF_INT128__)
#define CCM_TYPES_HAS_INT128
#endif // defined(__SIZEOF_INT128__)

// 'long double' properties.
#if (LDBL_MANT_DIG == 53)
#define CCM_TYPES_LONG_DOUBLE_IS_FLOAT64
#elif (LDBL_MANT_DIG == 64)
#define CCM_TYPES_LONG_DOUBLE_IS_FLOAT80
#elif (LDBL_MANT_DIG == 113)
#define CCM_TYPES_LONG_DOUBLE_IS_FLOAT128
#endif
