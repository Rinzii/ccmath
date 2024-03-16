/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#ifndef CCM_UNLIKELY
	#if defined(__cplusplus) && __cplusplus >= 202003L
		#define CCM_UNLIKELY(x) [[unlikely]] (x)
	#elif defined(__GNUC__) || defined(__clang__)
		#define CCM_UNLIKELY(x) __builtin_expect(!!(x), 0)
	#else
		#define CCM_UNLIKELY(x) (!!(x))
	#endif // defined(__cplusplus) && __cplusplus >= 202003L
#endif // CCM_UNLIKELY
