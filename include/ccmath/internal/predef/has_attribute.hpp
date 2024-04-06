/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

///
/// \def CCM_HAS_ATTR(x)
/// \brief This will check if a compiler has a specific attribute for GCC and Clang based compilers.
/// \note  Currently only supports GCC and Clang based compilers.
/// \note  This macro handles specifically checking for cpp attributes.
/// \param x The attribute to check for.
/// \return True if the compiler has the attribute, false otherwise.
///
#ifndef CCM_HAS_ATTR
	#if defined(__GNUC__) || defined(__clang__)
		#if defined(__has_attribute) // Check if the compiler has __has_attribute() support.
			// This will check if the compiler has a specific attribute. Only works for GCC and Clang based compilers.
			#define CCM_HAS_ATTR(x) __has_attribute(x)
		#elif defined(__has_cpp_attribute) // Attempt to fall back to __has_cpp_attribute if __has_attribute is not defined.
		// This will check if the compiler has a specific attribute. Only works for GCC and Clang based compilers.
			#define CCM_HAS_ATTR(x) __has_cpp_attribute(x)
		#elif defined(__has_c_attribute) // Attempt to fall back to __has_c_attribute if __has_cpp_attribute is not defined.
		// This will check if the compiler has a specific attribute. Only works for GCC and Clang based compilers.
			#define CCM_HAS_ATTR(x) __has_c_attribute(x)
		#else
		// If the compiler doesn't have __has_cpp_attribute() then we can't check for attributes in C++.
			#define CCM_HAS_ATTR(x) 0
		#endif
	#else
	// If not on a GCC or Clang based compiler then this macro does nothing.
		#define CCM_HAS_ATTR(x) 0
	#endif
#endif
