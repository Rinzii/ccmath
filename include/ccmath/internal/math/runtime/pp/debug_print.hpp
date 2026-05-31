/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

/*
 * Enables the ability to print debug messages to stderr for internal ccmath debugging.
 * Note: This will always be disabled if the cmake does not enable it.
 * ATTENTION: This code here is EXTREMELY finicky and might not work on every compiler.
 *			  It should NEVER be used outside of internal CCMath debugging and should always be wrapped.
 */

#pragma once

#if defined(CCMATH_CONFIG_ENABLE_DEBUG_PRINT)
	#if defined(__GNUC__) && !defined(__clang__)
extern "C" struct _IO_FILE;	  // Forward declaration allows us to avoid including cstdio.
extern "C" _IO_FILE * stderr; // Forward declaration allows us to avoid including cstdio.
		#define CCM_DEBUG_PRINT(msg, ...) __builtin_fprintf(stderr, msg, __VA_ARGS__)
	#elif defined(__clang__) && !defined(_MSC_VER)
		#define CCM_DEBUG_PRINT(...) __builtin_printf(__VA_ARGS__)
	#else
		#define CCM_DEBUG_PRINT(...) (-1)
	#endif
#else
	#define CCM_DEBUG_PRINT(...)
#endif
