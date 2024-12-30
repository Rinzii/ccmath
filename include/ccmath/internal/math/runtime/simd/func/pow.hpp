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

#include "ccmath/internal/config/arch/check_simd_support.hpp"

// Generic scalar implementation
#include "impl/scalar/pow.hpp"

#ifdef CCMATH_HAS_SIMD
	#ifdef CCMATH_HAS_SIMD_SSE2
		#include "impl/sse2/pow.hpp"
	#endif

	#ifdef CCMATH_HAS_SIMD_SSE3
		#include "impl/sse3/pow.hpp"
	#endif

	#ifdef CCMATH_HAS_SIMD_SSSE3
		#include "impl/ssse3/pow.hpp"
	#endif

	#ifdef CCMATH_HAS_SIMD_SSE4
		#include "impl/sse4/pow.hpp"
	#endif

	#ifdef CCMATH_HAS_SIMD_AVX
		#include "impl/avx/pow.hpp"
	#endif

	#ifdef CCMATH_HAS_SIMD_AVX2
		#include "impl/avx2/pow.hpp"
	#endif

	#ifdef CCMATH_HAS_SIMD_AVX512F
		#include "impl/avx512/pow.hpp"
	#endif

	// TODO: NEON does not have any builtin intrinsic for pow.
	//		 Need to implement this later.
	//#ifdef CCMATH_HAS_SIMD_NEON
	//	#include "impl/neon/pow.hpp"
	//#endif
#endif
