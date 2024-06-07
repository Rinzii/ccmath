/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

// Wrapper for handling including the correct intrinsics header file for the SIMD instruction set.

/*
 * We check for what to include in the order below. Always order these includes by the most modern to the least modern.
 * Arm Neon should always be last.
 *
 * Currently supported SIMD instruction sets:
 * - AVX
 * - SSE
 * - NEON
 */

#include "ccmath/internal/config/arch/check_simd_support.hpp"

#if defined(CCMATH_HAS_SIMD_AVX)
	#include <immintrin.h>
#elif defined(CCMATH_HAS_SIMD_SSE2)
	#include <immintrin.h>
#endif