/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

// TODO: Check this works on x86 and x86_64

/// x86 Architecture
// VS defines: _M_IX86
// GCC defines: i386, __i386, __i386__
#if defined(_M_IX86) || defined(__i386__) || defined(__i386) || defined(i386)
	#define CCM_TARGET_ARCH_X86
#endif

/// x86_64 Architecture
// VS defines: _M_X64 and _M_AMD64
// GCC defines: __amd64__, __amd64, __x86_64__, and __x86_64
#if defined(_M_X64) || defined(_M_AMD64) || defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
	#define CCM_TARGET_ARCH_X64
#endif