/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

// First check if we have access to the standard implementation of assume
#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(assume) >= 202207L
		#define ACCM_SSUME(...) [[assume(__VA_ARGS__)]]
	#endif
#endif

// If we don't have access to the standard implementation of [[assume()]] then try to check for compiler intrinsics/attributes
#ifndef CCM_ASSUME
// Be aware that CLion Nova's C++ Engine does not recognize __builtin_assume as a valid symbol.
// It is incorrect and this is a known bug that is fixed in CLion 2024.2.
// The code will work fine, so ignore the warning it may emit.
	#if defined(__clang__)
		#define CCM_ASSUME(...)                                                                                                                                \
			do {                                                                                                                                               \
				__builtin_assume(__VA_ARGS__);                                                                                                                 \
			} while (0)
	#elif defined(_MSC_VER)
		#define CCM_ASSUME(...)                                                                                                                                \
			do {                                                                                                                                               \
				__assume(__VA_ARGS__);                                                                                                                         \
			} while (0)
	#elif defined(__GNUC__)
		#if __GNUC__ >= 13
			#define CCM_ASSUME(...) __attribute__((__assume__(__VA_ARGS__)))
		#endif
	#endif
#endif

// If none of the above options are available, then just define CCM_ASSUME as empty
#ifndef CCM_ASSUME
	#define CCM_ASSUME(...)
#endif