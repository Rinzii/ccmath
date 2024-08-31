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

#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
	#ifdef CCM_TARGET_PLATFORM_WINDOWS
		#undef CCM_TARGET_PLATFORM_WINDOWS
	#endif
	#define CCM_TARGET_PLATFORM_WINDOWS 1

	#ifdef _WIN64 // VC++ defines both _WIN32 and _WIN64 when compiling for Win64.
		#define CCM_TARGET_PLATFORM_WINDOWS_64 1
	#else
		#define CCM_TARGET_PLATFORM_WINDOWS_32 1
	#endif

	// We assume a Windows device is a desktop device
	#ifdef CCM_TARGET_PLATFORM_DESKTOP
		#undef CCM_TARGET_PLATFORM_DESKTOP
	#endif

	#define CCM_TARGET_PLATFORM_DESKTOP 1
#endif
