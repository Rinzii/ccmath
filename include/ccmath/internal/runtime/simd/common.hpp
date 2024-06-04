/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

/**
 * Conventions followed:
 * - All macros with the name CCMATH_* are allowed to be defined internally and can be handled by the implementors.
 * - Macros that have the name CCM_CONFIG_* can NOT be defined by the user. These should be handled by our cmake script itself and never defined by us internally.
 */

#pragma once

#if !defined(CCMATH_FAST_MATH) && !defined(CCM_CONFIG_NO_FAST_MATH) && defined(__FAST_MATH__)
#define CCMATH_FAST_MATH
#endif

#if !defined(CCMATH_FAST_MATH) && !defined(CCM_CONFIG_NO_FAST_NANS)
	#if defined(CCMATH_FAST_MATH)
		#define CCMATH_FAST_NANS
	#elif defined(__FINITE_MATH_ONLY__)
		#if __FINITE_MATH_ONLY__
			#define CCMATH_FAST_NANS
		#endif
	#endif
#endif

if constexpr (std::is_same_v<T, float>)
{
	__m128 num_m		= _mm_set_ss(num);
	__m128 const sqrt_m = _mm_sqrt_ss(num_m);
	return _mm_cvtss_f32(sqrt_m);
}
if constexpr (std::is_same_v<T, double>)
{
	__m128d num_m		 = _mm_set_sd(num);
	__m128d const sqrt_m = _mm_sqrt_sd(num_m, num_m);
	return _mm_cvtsd_f64(sqrt_m);
}

