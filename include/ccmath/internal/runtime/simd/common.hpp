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

#include "ccmath/internal/predef/has_attribute.hpp"
#include "ccmath/internal/predef/versioning/gcc_version.hpp"

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

#if (CCM_HAS_ATTR(may_alias) && (!defined(__SUNPRO_C) && !defined(__SUNPRO_CC))) || CCM_GCC_VERSION_CHECK(3,3,0)
	#define CCM_ATTR_MAY_ALIAS __attribute__((__may_alias__))
#else
	#define CCM_ATTR_MAY_ALIAS
#endif


#if CCM_GCC_VERSION_CHECK(4,8,0)
	#define CCM_VECTOR(size) __attribute__((__vector_size__(size)))
	#define CCMATH_VECTOR_OPS
	#define CCMATH_VECTOR_NEGATE
	#define CCMATH_VECTOR_SCALAR
	#define CCMATH_VECTOR_SUBSCRIPT
#else
	#define CCM_VECTOR(size)
#endif

/*

#if \
  (HEDLEY_HAS_ATTRIBUTE(may_alias) && !defined(HEDLEY_SUNPRO_VERSION)) || \
  HEDLEY_GCC_VERSION_CHECK(3,3,0) || \
  HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
  HEDLEY_IBM_VERSION_CHECK(13,1,0)
	#  define SIMDE_MAY_ALIAS __attribute__((__may_alias__))
#else
	#  define SIMDE_MAY_ALIAS
#endif

#if !defined(SIMDE_NO_VECTOR)
	#  if \
    HEDLEY_GCC_VERSION_CHECK(4,8,0)
		#    define SIMDE_VECTOR(size) __attribute__((__vector_size__(size)))
		#    define SIMDE_VECTOR_OPS
		#    define SIMDE_VECTOR_NEGATE
		#    define SIMDE_VECTOR_SCALAR
		#    define SIMDE_VECTOR_SUBSCRIPT
	#  elif HEDLEY_INTEL_VERSION_CHECK(16,0,0)
		#    define SIMDE_VECTOR(size) __attribute__((__vector_size__(size)))
		#    define SIMDE_VECTOR_OPS
		#    define SIMDE_VECTOR_NEGATE
		#    define SIMDE_VECTOR_SUBSCRIPT
	#  elif \
    HEDLEY_GCC_VERSION_CHECK(4,1,0) || \
    HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
		#    define SIMDE_VECTOR(size) __attribute__((__vector_size__(size)))
		#    define SIMDE_VECTOR_OPS
	#  elif HEDLEY_SUNPRO_VERSION_CHECK(5,12,0)
		#    define SIMDE_VECTOR(size) __attribute__((__vector_size__(size)))
	#  elif HEDLEY_HAS_ATTRIBUTE(vector_size)
		#    define SIMDE_VECTOR(size) __attribute__((__vector_size__(size)))
		#    define SIMDE_VECTOR_OPS
		#    define SIMDE_VECTOR_NEGATE
		#    define SIMDE_VECTOR_SUBSCRIPT
		#    if SIMDE_DETECT_CLANG_VERSION_CHECK(5,0,0)
			#      define SIMDE_VECTOR_SCALAR
		#    endif
	#  endif
 */