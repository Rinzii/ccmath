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
#include "ccmath/internal/math/runtime/simd/func/impl/scalar/map.hpp"
#include "ccmath/internal/math/runtime/simd/func/impl/scalar/pow.hpp"
#include "ccmath/internal/math/runtime/simd/func/impl/scalar/sqrt.hpp"
#include "ccmath/internal/math/runtime/simd/func/pow.hpp"
#include "ccmath/internal/math/runtime/simd/func/sqrt.hpp"
#include "ccmath/internal/math/runtime/simd/func/svml_ops.hpp"

#ifdef CCMATH_HAS_SIMD
	#ifdef CCMATH_HAS_SIMD_SSE2
		#include "ccmath/internal/math/runtime/simd/func/impl/sse2/pow.hpp"
		#include "ccmath/internal/math/runtime/simd/func/impl/sse2/sqrt.hpp"
	#endif

	#ifdef CCMATH_HAS_SIMD_SSE4
		#include "ccmath/internal/math/runtime/simd/func/impl/sse4/pow.hpp"
		#include "ccmath/internal/math/runtime/simd/func/impl/sse4/sqrt.hpp"
	#endif

	#ifdef CCMATH_HAS_SIMD_AVX2
		#include "ccmath/internal/math/runtime/simd/func/impl/avx2/pow.hpp"
		#include "ccmath/internal/math/runtime/simd/func/impl/avx2/sqrt.hpp"
	#endif

	#ifdef CCMATH_HAS_SIMD_NEON
		#include "ccmath/internal/math/runtime/simd/func/impl/neon/pow.hpp"
		#include "ccmath/internal/math/runtime/simd/func/impl/neon/sqrt.hpp"
	#endif

	#ifdef CCMATH_HAS_SIMD_ENABLE_VECTOR_SIZE
		#include "ccmath/internal/math/runtime/simd/func/impl/vector_size/sqrt.hpp"
	#endif
#endif
