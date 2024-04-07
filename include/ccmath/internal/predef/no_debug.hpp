/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/predef/has_attribute.hpp"

#ifndef CCM_NODEBUG
	#if defined(__GNUC__) || defined(__clang__)
		#if CCM_HAS_ATTR(__nodebug__)
			#define CCM_NODEBUG __attribute__((__nodebug__))
		#else
			#define CCM_NODEBUG
		#endif
	#else
		#define CCM_NODEBUG
	#endif
#endif

///
/// \def CCM_HAS_DECLSPEC_ATTR(x)
/// \brief This function-like macro takes a single identifier argument that is the name of an attribute implemented as a Microsoft-style __declspec attribute.
/// \note  MSVC will always return \a true for this macro.
/// \note  Currently this macro is supported only by clang based compilers and will return \a false on non-clang based compilers.
/// \param x The attribute to check for.
/// \return True if the compiler has the attribute, false otherwise.
///
#ifndef CCM_HAS_DECLSPEC_ATTR
	#if defined(_MSC_VER)
		// MSVC will always return true for this macro.
		#define CCM_HAS_DECLSPEC_ATTR(x) 1
	#elif defined(__clang__)
		#if defined(__has_declspec_attribute)
		// This will check if the compiler has access to a specific declspec attribute. Only works for Clang based compilers.
			#define CCM_HAS_DECLSPEC_ATTR(x) __has_declspec_attribute(x)
		#else
		// If the compiler doesn't have __has_declspec_attribute() then we can't check for declspec attributes in C++.
			#define CCM_HAS_DECLSPEC_ATTR(x) 0
		#endif
	#else
	// If not on a Clang based compiler then this macro does nothing.
		#define CCM_HAS_DECLSPEC_ATTR(x) 0
	#endif
#endif
