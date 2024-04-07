/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <cfloat>

// Mirror float_t and double_t from <cmath> to avoid having a dependency of <cmath>.

#ifdef FLT_EVAL_METHOD
	# if FLT_EVAL_METHOD == -1
		#  define CCM_FLT_EVAL_METHOD	2
	# else
		#  define CCM_FLT_EVAL_METHOD	FLT_EVAL_METHOD
	# endif
#elif defined __x86_64__
	# define CCM_FLT_EVAL_METHOD	0
#else
	# define CCM_FLT_EVAL_METHOD	2
#endif

namespace ccm
{
#if CCM_FLT_EVAL_METHOD == 0 || CCM_FLT_EVAL_METHOD == 16
	using float_t = float;
	using double_t = double;
#elif CCM_FLT_EVAL_METHOD == 1
	typedef double float_t;
	typedef double double_t;
#elif CCM_FLT_EVAL_METHOD == 2
	typedef long double float_t;
	typedef long double double_t;
# else
	#  error "Unknown CCM_FLT_EVAL_METHOD"
# endif
}
