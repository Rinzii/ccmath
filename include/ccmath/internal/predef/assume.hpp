/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

// Check if c++23
#if __cplusplus >= 202302L
	#define CCM_ASSUME(cond) [[assume(cond)]]
#elif defined(_MSC_VER)
	#define CCM_ASSUME(cond) __assume(cond)
#elif defined(__has_builtin) && __has_builtin(__builtin_assume)
    #define CCM_ASSUME(cond) __builtin_assume(cond)
#elif defined(__GNUC__)
	#define CCM_ASSUME(cond) ((cond) ? static_cast<void>(0) : __builtin_unreachable())
#else
	#define CCM_ASSUME(cond) static_cast<void>((cond) ? 0 : 0)
#endif // __cplusplus >= 202302L
