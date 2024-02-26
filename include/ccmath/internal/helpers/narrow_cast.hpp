/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <utility>


//
// make suppress attributes parse for some compilers
// Hopefully temporary until suppression standardization occurs
//
#if defined(__clang__)
	#define CCM_SUPPRESS(x) [[gsl::suppress(#x)]]
#else
	#if defined(_MSC_VER) && !defined(__INTEL_COMPILER) && !defined(__NVCC__)
		#define CCM_SUPPRESS(x) [[gsl::suppress(x)]]
	#else
		#define CCM_SUPPRESS(x)
	#endif // _MSC_VER
#endif // __clang__



namespace ccm::internal
{
	// NOLINTBEGIN

	// disable narrowing
#ifdef __GNUC__
	_Pragma("GCC diagnostic push")
    _Pragma("GCC diagnostic ignored \"-Wnarrowing\"")
#endif
#ifdef __clang__
	_Pragma("clang diagnostic push")
    _Pragma("clang diagnostic ignored \"-Wc++11-narrowing\"")
#endif
	template <typename To, typename From>
	CCM_SUPPRESS(type.1) // NO-FORMAT: attribute
	inline constexpr To narrow_cast(From&& from) noexcept
	{
		return static_cast<To>(std::forward<From>(from));
	}
#ifdef __GNUC__
    _Pragma("GCC diagnostic pop")
#endif
#ifdef __clang__
	_Pragma("clang diagnostic pop")
#endif
	// NOLINTEND

}


#undef GSL_SUPPRESS
