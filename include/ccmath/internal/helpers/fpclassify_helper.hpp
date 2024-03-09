/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

// Implementation of cmath floating point macros based on supported compilers

// TODO: Add support for other compilers

// Currently supported compilers:
// - GCC
// - Clang
// - Apple Clang
// - MSVC
// - Intel DPC++
// - Nvidia HPC SDK

// Identify the compiler
#include "ccmath/internal/setup/compiler_def.hpp"

namespace ccm::helpers
{
	enum class floating_point_defines
    {
#if defined(CCMATH_COMPILER_MSVC) // Mirrors the corecrt definitions
        eFP_NAN = 2,
		eFP_INFINITE = 1,
		eFP_ZERO = 0,
		eFP_SUBNORMAL = -2,
		eFP_NORMAL = -1
#elif defined(CCMATH_COMPILER_APPLE_CLANG) // Apple Clang has a different set of defines than Clang
        eFP_NAN = 1,
        eFP_INFINITE = 2,
        eFP_ZERO = 3,
        eFP_SUBNORMAL = 5,
        eFP_NORMAL = 4,
#elif defined(CCMATH_COMPILER_CLANG) || defined(CCMATH_COMPILER_GCC) || defined(CCMATH_COMPILER_CLANG_BASED) // Clang and GCC have the same defines
        eFP_NAN = 0,
        eFP_INFINITE = 1,
        eFP_ZERO = 2,
        eFP_SUBNORMAL = 3,
        eFP_NORMAL = 4,
#else // Unknown compiler throws a static_assert. Since FP_* macros are extremely implementation specific, we can't provide a default.
        eFP_NAN = 0,
        eFP_INFINITE,
        eFP_ZERO,
        eFP_SUBNORMAL,
        eFP_NORMAL
		static_assert(false, "FP_* macros are extremely implementation specific and are not defined for this compiler. Please add support for this compiler.")
#endif
    };
}

// Clean up the global namespace
//#include "ccmath/internal/setup/compiler_undef.hpp"

