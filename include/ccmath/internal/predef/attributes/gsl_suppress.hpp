/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
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
