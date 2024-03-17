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
#include "ccmath/internal/config/compiler_def.hpp"

namespace ccm::helpers
{
	struct floating_point_defines
	{
#if defined(CCMATH_COMPILER_MSVC) // Mirrors the corecrt definitions
		constexpr static int eFP_NAN = 2;
		constexpr static int eFP_INFINITE = 1;
		constexpr static int eFP_ZERO = 0;
		constexpr static int eFP_SUBNORMAL = -2;
		constexpr static int eFP_NORMAL = -1;
#elif defined(CCMATH_COMPILER_APPLE_CLANG) // Apple Clang has a different set of defines than Clang
		constexpr static int eFP_NAN = 1;
		constexpr static int eFP_INFINITE = 2;
		constexpr static int eFP_ZERO = 3;
		constexpr static int eFP_SUBNORMAL = 5;
		constexpr static int eFP_NORMAL = 4;
#elif defined(CCMATH_COMPILER_NVIDIA_HPC) // Nvidia HPC SDK has a different set of defines than Clang
		constexpr static int eFP_NAN = 0;
		constexpr static int eFP_INFINITE = 1;
		constexpr static int eFP_ZERO = 2;
		constexpr static int eFP_SUBNORMAL = 2;
		constexpr static int eFP_NORMAL = 4;
#elif defined(CCMATH_COMPILER_CLANG) || defined(CCMATH_COMPILER_GCC) || defined(CCMATH_COMPILER_CLANG_BASED) // Clang and GCC have the same defines
		constexpr static int eFP_NAN = 0;
		constexpr static int eFP_INFINITE = 1;
		constexpr static int eFP_ZERO = 2;
		constexpr static int eFP_SUBNORMAL = 3;
		constexpr static int eFP_NORMAL = 4;
#else
		static_assert(false, "FP_* macros are extremely implementation specific and are not defined for this compiler. Please add support for this compiler.")
#endif
	};
}

// Clean up the global namespace
#include "ccmath/internal/config/compiler_undef.hpp"

