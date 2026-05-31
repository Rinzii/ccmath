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

#if defined(__linux) || defined(__linux__)

	// Check that what we plan to define is not already defined
	#ifdef CCM_TARGET_PLATFORM_LINUX
		#undef CCM_TARGET_PLATFORM_LINUX
	#endif
	#ifdef CCM_TARGET_PLATFORM_UNIX
		#undef CCM_TARGET_PLATFORM_UNIX
	#endif
	#ifdef CCM_TARGET_PLATFORM_POSIX
		#undef CCM_TARGET_PLATFORM_POSIX
	#endif

	#define CCM_TARGET_PLATFORM_LINUX 1
	#define CCM_TARGET_PLATFORM_UNIX  1
	#define CCM_TARGET_PLATFORM_POSIX 1

	#ifdef CCM_TARGET_PLATFORM_DESKTOP
		#undef CCM_TARGET_PLATFORM_DESKTOP
	#endif

	#define CCM_TARGET_PLATFORM_DESKTOP 1
#endif
