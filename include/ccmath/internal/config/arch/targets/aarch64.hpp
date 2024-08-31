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

// TODO: Check this works on aarch32 and aarch64

/// ARM Architecture
// VS defines: _M_ARM
// GCC defines: __arm__
#if defined(_M_ARM) || defined(__arm__)
	#define CCM_TARGET_ARCH_AARCH32 3
	#define CCM_TARGET_ARCH_IS_ARM_BASED 1
#endif

/// ARM64 Architecture
// VS defines: _M_ARM64
// GCC defines: __aarch64__
#if defined(_M_ARM64) || defined(__aarch64__)
	#define CCM_TARGET_ARCH_AARCH64 4
	#define CCM_TARGET_ARCH_IS_ARM_BASED 1
#endif


