/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/arch/check_simd_support.hpp"
#include "ccmath/internal/config/arch/check_arch_support.hpp"

#if !defined(CCMATH_SIMD_ARM_NEON_A64V8_NATIVE)
	#if defined(CCMATH_SIMD_ARCH_ARM_NEON) && defined(CCMATH_ARCH_AARCH64) && CCMATH_SIMD_ARCH_ARM_CHECK(8,0)
		#define CCMATH_SIMD_ARM_NEON_A64V8_NATIVE
	#endif
#endif
#if defined(CCMATH_SIMD_ARM_NEON_A64V8_NATIVE) && !defined(CCMATH_SIMD_ARM_NEON_A32V8_NATIVE)
	#define CCMATH_SIMD_ARM_NEON_A32V8_NATIVE
#endif

#if !defined(CCMATH_SIMD_ARM_NEON_A32V8_NATIVE) && !defined(CCMATH_SIMD_ARM_NEON_A32V8_NO_NATIVE) && !defined(CCMATH_SIMD_NO_NATIVE)
	#if defined(CCMATH_SIMD_ARCH_ARM_NEON) && CCMATH_SIMD_ARCH_ARM_CHECK(8,0) && defined (__ARM_NEON_FP) && (__ARM_NEON_FP & 0x02)
		#define CCMATH_SIMD_ARM_NEON_A32V8_NATIVE
	#endif
#endif
#if defined(__ARM_ACLE)
	#include <arm_acle.h>
#endif
#if defined(CCMATH_SIMD_ARM_NEON_A32V8_NATIVE) && !defined(CCMATH_SIMD_ARM_NEON_A32V7_NATIVE)
	#define CCMATH_SIMD_ARM_NEON_A32V7_NATIVE
#endif

#if !defined(CCMATH_SIMD_ARM_NEON_A32V7_NATIVE) && !defined(CCMATH_SIMD_ARM_NEON_A32V7_NO_NATIVE) && !defined(CCMATH_SIMD_NO_NATIVE)
	#if defined(CCMATH_SIMD_ARCH_ARM_NEON) && CCMATH_SIMD_ARCH_ARM_CHECK(7,0)
		#define CCMATH_SIMD_ARM_NEON_A32V7_NATIVE
	#endif
#endif
#if defined(CCMATH_SIMD_ARM_NEON_A32V7_NATIVE)
	#include <arm_neon.h>
	#if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC)
		#include <arm_fp16.h>
	#endif
#endif