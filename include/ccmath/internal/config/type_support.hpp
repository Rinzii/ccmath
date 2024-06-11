/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <cfloat>  // LDBL_MANT_DIG
#include <cstdint> // UINT64_MAX, __SIZEOF_INT128__

#if defined(UINT64_MAX)
#define CCM_TYPES_HAS_INT64
#endif // UINT64_MAX

// Checks whether the __int128 compiler extension for a 128-bit integral type is
// supported.
//
// Note: __SIZEOF_INT128__ is defined by Clang and GCC when __int128 is
// supported, but we avoid using it in certain cases:
// * On Clang:
//   * Building using Clang for Windows, where the Clang runtime library has
//     128-bit support only on LP64 architectures, but Windows is LLP64.
// * On Nvidia's nvcc:
//   * nvcc also defines __GNUC__ and __SIZEOF_INT128__, but not all versions
//     actually support __int128.
#ifdef CCM_TYPES_HAS_INT128
	#error CCM_TYPES_HAS_INT128 cannot be directly set
#elif defined(__SIZEOF_INT128__)
	#if (defined(__clang__) && !defined(_WIN32)) || (defined(__CUDACC__) && __CUDACC_VER_MAJOR__ >= 9) ||                                                      \
		(defined(__GNUC__) && !defined(__clang__) && !defined(__CUDACC__))
		#define CCM_TYPES_HAS_INT128 1
	#elif defined(__CUDACC__)
		// __CUDACC_VER__ is a full version number before CUDA 9, and is defined to a
		// string explaining that it has been removed starting with CUDA 9. We use
		// nested #ifs because there is no short-circuiting in the preprocessor.
		// NOTE: `__CUDACC__` could be undefined while `__CUDACC_VER__` is defined.
		#if __CUDACC_VER__ >= 70000
			#define CCM_TYPES_HAS_INT128 1
		#endif // __CUDACC_VER__ >= 70000
	#endif	   // defined(__CUDACC__)
#endif		   // CCM_TYPES_HAS_INT128

// TODO: MSVC v19.34 has added support for int128 internally.
// These should work with constexpr and should use intrinsics to speed up when its able to.
// std::_Unsigned128
// std::_Signed128
// #include __msvc_int128.hpp


// 'long double' properties.
#if (LDBL_MANT_DIG == 53)
	#define CCM_TYPES_LONG_DOUBLE_IS_FLOAT64
#elif (LDBL_MANT_DIG == 64)
	#define CCM_TYPES_LONG_DOUBLE_IS_FLOAT80
#elif (LDBL_MANT_DIG == 113)
	#define CCM_TYPES_LONG_DOUBLE_IS_FLOAT128
#endif
