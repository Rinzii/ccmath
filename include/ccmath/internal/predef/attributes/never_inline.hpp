/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#ifndef CCM_NEVER_INLINE
	#if defined(__GNUC__) || defined(__clang__)
		#define CCM_NEVER_INLINE __attribute__((noinline))
	#elif defined(_MSC_VER)
		#define CCM_NEVER_INLINE __declspec(noinline)
	#else
		#define CCM_NEVER_INLINE
	#endif
#endif
