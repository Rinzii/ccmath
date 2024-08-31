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

// Implementation of cmath floating point macros based on supported compilers

// Currently supported compilers:
// - GCC
// - Clang
// - Apple Clang
// - MSVC
// - Intel DPC++
// - Nvidia HPC SDK

#include "ccmath/internal/config/compiler.hpp" // Identify the compiler
#include "ccmath/internal/support/always_false.hpp"

namespace ccm::support::helpers
{
	struct floating_point_defines
	{
#if defined(CCMATH_COMPILER_MSVC) // Mirrors the corecrt definitions
		static constexpr int eFP_NAN = 2;
		static constexpr int eFP_INFINITE = 1;
		static constexpr int eFP_ZERO = 0;
		static constexpr int eFP_SUBNORMAL = -2;
		static constexpr int eFP_NORMAL = -1;
#elif defined(CCMATH_COMPILER_APPLE_CLANG) // Apple Clang has a different set of defines than Clang
		static constexpr int eFP_NAN = 1;
		static constexpr int eFP_INFINITE = 2;
		static constexpr int eFP_ZERO = 3;
		static constexpr int eFP_SUBNORMAL = 5;
		static constexpr int eFP_NORMAL = 4;
#elif defined(CCMATH_COMPILER_NVIDIA_HPC) // Nvidia HPC SDK has a different set of defines than GCC
		static constexpr int eFP_NAN = 0;
		static constexpr int eFP_INFINITE = 1;
		static constexpr int eFP_ZERO = 2;
		static constexpr int eFP_SUBNORMAL = 2;
		static constexpr int eFP_NORMAL = 4;
#elif defined(CCMATH_COMPILER_CLANG) || defined(CCMATH_COMPILER_GCC) || defined(CCMATH_COMPILER_CLANG_BASED) // Clang and GCC have the same defines
		static constexpr int eFP_NAN = 0;
		static constexpr int eFP_INFINITE = 1;
		static constexpr int eFP_ZERO = 2;
		static constexpr int eFP_SUBNORMAL = 3;
		static constexpr int eFP_NORMAL = 4;
#else
		#error "FP_* macros are extremely implementation specific and are not defined for this compiler. Please add support for this compiler."
#endif
	};
} // namespace ccm::helpers
