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

#ifndef CCM_NEVER_INLINE
	#if defined(__GNUC__) || defined(__clang__)
		#define CCM_NEVER_INLINE __attribute__((noinline))
	#elif defined(_MSC_VER)
		#define CCM_NEVER_INLINE __declspec(noinline)
	#else
		#define CCM_NEVER_INLINE
	#endif
#endif
