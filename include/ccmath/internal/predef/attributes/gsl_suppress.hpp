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

#if defined(__clang__)
	#define CCM_SUPPRESS(x) [[gsl::suppress("x")]]
#else
	#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
		#define CCM_SUPPRESS(x) [[gsl::suppress(x)]]
	#else
		#define CCM_SUPPRESS(x)
	#endif // _MSC_VER
#endif // __clang__
