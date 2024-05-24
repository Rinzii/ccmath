/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once


#if defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>

	// Apple family of operating systems.

	// Check that what we plan to define is not already defined
	#ifdef CCM_TARGET_PLATFORM_APPLE
		#undef CCM_TARGET_PLATFORM_APPLE
	#endif
	#ifdef CCM_TARGET_PLATFORM_POSIX
		#undef CCM_TARGET_PLATFORM_POSIX
	#endif

	#define CCM_TARGET_PLATFORM_APPLE 1
	#define CCM_TARGET_PLATFORM_POSIX 1

	// iPhone
	// TARGET_OS_IPHONE will be defined on gcc.
	#if defined(__IPHONE__) || (defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE)

		// Check that what we plan to define is not already defined
		#ifdef CCM_TARGET_PLATFORM_IPHONE
			#undef CCM_TARGET_PLATFORM_IPHONE
		#endif
		#define CCM_TARGET_PLATFORM_IPHONE 1
		#define CCM_TARGET_POSIX_THREADS_AVAILABLE 1

		#ifdef CCM_TARGET_PLATFORM_MOBILE
			#undef CCM_TARGET_PLATFORM_MOBILE
		#endif
		#define CCM_TARGET_PLATFORM_MOBILE 1

	// Macintosh OSX
	// __MACH__ is defined by the GCC compiler.
	#elif defined(__MACH__)

		// Check that what we plan to define is not already defined
		#ifdef CCM_TARGET_PLATFORM_OSX
			#undef CCM_TARGET_PLATFORM_OSX
		#endif
		#ifdef CCM_TARGET_PLATFORM_UNIX
			#undef CCM_TARGET_PLATFORM_UNIX
		#endif
		#ifdef CCM_TARGET_PLATFORM_POSIX
			#undef CCM_TARGET_PLATFORM_POSIX
		#endif

		#define CCM_TARGET_PLATFORM_OSX 1
		#define CCM_TARGET_PLATFORM_UNIX 1
		#define CCM_TARGET_PLATFORM_POSIX 1

		#ifdef CCM_TARGET_PLATFORM_DESKTOP
			#undef CCM_TARGET_PLATFORM_DESKTOP
		#endif

		#define CCM_TARGET_PLATFORM_DESKTOP 1
	#else
		#error Unknow Apple Platform
	#endif
#endif