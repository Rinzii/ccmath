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

#include "ccmath/internal/predef/has_builtin.hpp"

#if CCM_HAS_BUILTIN(__builtin_is_constant_evaluated)
	#define CCMATH_HAS_BUILTIN_IS_CONSTANT_EVALUATED
#endif

// GCC/Clang 9 and later has __builtin_is_constant_evaluated
#if (defined(__GNUC__) && (__GNUC__ >= 9)) || (defined(__clang__) && (__clang_major__ >= 9))
	#define CCMATH_HAS_BUILTIN_IS_CONSTANT_EVALUATED
#endif

// Visual Studio 2019 and later supports __builtin_is_constant_evaluated
#if defined(_MSC_FULL_VER) && (_MSC_FULL_VER >= 192528326)
	#  define CCMATH_HAS_BUILTIN_IS_CONSTANT_EVALUATED
#endif

namespace ccm::support
{
	constexpr bool is_constant_evaluated() noexcept
	{
		#if defined(CCMATH_HAS_BUILTIN_IS_CONSTANT_EVALUATED)
		return __builtin_is_constant_evaluated();
		#else
		return false;
		#endif
	}
} // namespace ccm::support
