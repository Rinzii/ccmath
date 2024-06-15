/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/arch/check_simd_support.hpp"

// Generic scalar implementation
#include "impl/scalar/sqrt.hpp"

#ifdef CCMATH_SIMD
	#ifdef CCMATH_SIMD_SSE2
		#include "impl/sse2/sqrt.hpp"
	#endif

	#ifdef CCMATH_SIMD_AVX
		#include "impl/avx/sqrt.hpp"
	#endif

	#ifdef CCMATH_SIMD_AVX512F
		#include "impl/avx512/sqrt.hpp"
	#endif

	#ifdef CCMATH_SIMD_NEON
		#include "impl/neon/sqrt.hpp"
	#endif

	#ifdef CCMATH_SIMD_ENABLE_VECTOR_SIZE
		#include "impl/vector_size/sqrt.hpp"
	#endif
#endif