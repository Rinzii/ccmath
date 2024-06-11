/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

// TODO: Check this works on aarch32 and aarch64

/// ARM Architecture
// VS defines: _M_ARM
// GCC defines: __arm__
#if defined(_M_ARM) || defined(__arm__)
	#define CCM_TARGET_ARCH_AARCH32
#endif

/// ARM64 Architecture
// VS defines: _M_ARM64
// GCC defines: __aarch64__
#if defined(_M_ARM64) || defined(__aarch64__)
	#define CCM_TARGET_ARCH_AARCH64
#endif


