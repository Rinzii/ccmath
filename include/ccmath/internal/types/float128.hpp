/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once


namespace ccm::types
{

// Check for the availability of C23's _Float128 type.
// Currently, _Float128 is available in GCC 7 or later when compiling in C mode.
// Clang does not define _Float128 and instead uses __float128, and only on x86-64 targets.
#if defined(__STDC_IEC_60559_BFP__) && !defined(__clang__) && !defined(__cplusplus)
	#define CCM_TYPES_HAS_FLOAT128
	typedef _Float128 float128;

// If _Float128 is not available, check for __float128.
// Occasionally, __SIZEOF_FLOAT128__ is defined by gcc and clang to notify the
// availability of __float128. Clang also defines the macro __FLOAT128__ to
// identify the availability of __float128 type. As specified here: https://reviews.llvm.org/D15120
#elif defined(__FLOAT128__) || defined(__SIZEOF_FLOAT128__)
	#define CCM_TYPES_HAS_FLOAT128
	using float128 = __float128;

// If neither _Float128 nor __float128 are available, check if long double is 128 bits.
#elif (LDBL_MANT_DIG == 113)
	#define CCM_TYPES_HAS_FLOAT128
	using float128 = long double;
#endif

} // namespace ccm::types