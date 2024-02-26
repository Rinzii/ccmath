/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present cmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

// Note: This is used to generate the CCM_COMPILER_STRING macros
#ifndef INTERNAL_STRINGIFY
	#define INTERNAL_STRINGIFY(x) INTERNAL_PRIMITIVE_STRINGIFY(x)
#endif
#ifndef INTERNAL_PRIMITIVE_STRINGIFY
	#define INTERNAL_PRIMITIVE_STRINGIFY(x) #x
#endif

/// CCM_COMPILER_CPP20_ENABLED
// This check could have some issues with some compilers as a few have decided not to
// define __cplusplus correctly until they are fully supporting C++20, but this should
// not matter to use for our purposes.
#if !defined(CCM_COMPILER_CPP20_ENABLED) && defined(__cplusplus)
	#if (__cplusplus >= 202002L)
		#define CCM_COMPILER_CPP20_ENABLED 1
	#elif defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L) // C++20+
		#define CCM_COMPILER_CPP20_ENABLED 1
	#endif
#endif

// Clang's GCC-compatible driver.
#if defined(__clang__) && !defined(_MSC_VER)
	#define CCM_COMPILER_CLANG		 1
	#define CCM_COMPILER_CLANG_BASED 1
	#define CCM_COMPILER_VERSION	 (__clang_major__ * 100 + __clang_minor__ * 10 + __clang_patchlevel__)

	#if defined(__apple_build_version__)
		#define CCM_COMPILER_APPLE_CLANG 1
		#define CCM_COMPILER_CLANG_BASED 1
	#endif

// GCC (a.k.a. GNUC)
#elif defined(__GNUC__)
	#define CCM_COMPILER_GCC	 1
	#define CCM_COMPILER_VERSION (__GNUC__ * 100 + __GNUC_MINOR__ * 10 + __GNUC_PATCHLEVEL__)

#elif defined(_MSC_VER)
	#define CCM_COMPILER_MSVC	 1
	#define CCM_COMPILER_VERSION _MSC_VER

	#if defined(__clang__)
	// Clang's MSVC-compatible driver.
		#define CCM_COMPILER_CLANG_CL 1
	#endif
#endif

// ------------------------------------------------------------------------
// CCM_DISABLE_GCC_WARNING / CCM_RESTORE_GCC_WARNING
//
// Example usage:
//     // Only one warning can be ignored per statement, due to how GCC works.
//     CCM_DISABLE_GCC_WARNING(-Wuninitialized)
//     CCM_DISABLE_GCC_WARNING(-Wunused)
//     <code>
//     CCM_RESTORE_GCC_WARNING()
//     CCM_RESTORE_GCC_WARNING()
//
#ifndef CCM_DISABLE_GCC_WARNING
	#if defined(CCM_COMPILER_GCC)
		#define CCMGCCWHELP0(x) #x
		#define CCMGCCWHELP1(x) CCMGCCWHELP0(GCC diagnostic ignored x)
		#define CCMGCCWHELP2(x) CCMGCCWHELP1(#x)
	#endif

	#if defined(CCM_COMPILER_GCC) && (CCM_COMPILER_VERSION >= 4006) // Can't test directly for __GNUC__ because some compilers lie.
		#define CCM_DISABLE_GCC_WARNING(w) _Pragma("GCC diagnostic push") _Pragma(CCMGCCWHELP2(w))
	#else
		#define CCM_DISABLE_GCC_WARNING(w)
	#endif
#endif

#ifndef CCM_RESTORE_GCC_WARNING
	#if defined(CCM_COMPILER_GCC) && (CCM_COMPILER_VERSION >= 4006)
		#define CCM_RESTORE_GCC_WARNING() _Pragma("GCC diagnostic pop")
	#else
		#define CCM_RESTORE_GCC_WARNING()
	#endif
#endif

// ------------------------------------------------------------------------
// CCM_ENABLE_GCC_WARNING_AS_ERROR / CCM_DISABLE_GCC_WARNING_AS_ERROR
//
// Example usage:
//     // Only one warning can be treated as an error per statement, due to how GCC works.
//     CCM_ENABLE_GCC_WARNING_AS_ERROR(-Wuninitialized)
//     CCM_ENABLE_GCC_WARNING_AS_ERROR(-Wunused)
//     <code>
//     CCM_DISABLE_GCC_WARNING_AS_ERROR()
//     CCM_DISABLE_GCC_WARNING_AS_ERROR()
//
#ifndef CCM_ENABLE_GCC_WARNING_AS_ERROR
	#if defined(CCM_COMPILER_GCC)
		#define CCMGCCWERRORHELP0(x) #x
		#define CCMGCCWERRORHELP1(x) CCMGCCWERRORHELP0(GCC diagnostic error x)
		#define CCMGCCWERRORHELP2(x) CCMGCCWERRORHELP1(#x)
	#endif
#endif

#if defined(CCM_COMPILER_GCC) && (CCM_COMPILER_VERSION >= 4006) // Can't test directly for __GNUC__ because some compilers lie.
	#define CCM_ENABLE_GCC_WARNING_AS_ERROR(w) _Pragma("GCC diagnostic push") _Pragma(CCMGCCWERRORHELP2(w))
#else
	#define CCM_ENABLE_GCC_WARNING_AS_ERROR(w)
#endif

#ifndef CCM_DISABLE_GCC_WARNING_AS_ERROR
	#if defined(CCM_COMPILER_GCC) && (CCM_COMPILER_VERSION >= 4006)
		#define CCM_DISABLE_GCC_WARNING_AS_ERROR() _Pragma("GCC diagnostic pop")
	#else
		#define CCM_DISABLE_GCC_WARNING_AS_ERROR()
	#endif
#endif

#ifndef CCM_ENABLE_IGNORE_GCC_WARNING
	#if defined(CCM_COMPILER_GCC)
		#define CCMGCCIGNOREHELP0(x) #x
		#define CCMGCCIGNOREHELP1(x) CCMGCCIGNOREHELP0(GCC diagnostic ignored x)
		#define CCMGCCIGNOREHELP2(x) CCMGCCIGNOREHELP1(#x)
	#endif
#endif

#ifndef CCM_ENABLE_IGNORE_GCC_WARNING
	#if defined(CCM_COMPILER_GNUC) && (CCM_COMPILER_VERSION >= 4006) // Can't test directly for __GNUC__ because some compilers lie.
		#define CCM_ENABLE_IGNORE_GCC_WARNING(w) _Pragma("GCC diagnostic push") _Pragma(CCMGCCIGNOREHELP2(w))
	#else
		#define CCM_ENABLE_IGNORE_GCC_WARNING(w)
	#endif
#endif

#ifndef CCM_DISABLE_IGNORE_GCC_WARNING
	#if defined(CCM_COMPILER_GCC)
		#define CCMGCCIGNOREDISABLEHELP0(x) #x
		#define CCMGCCIGNOREDISABLEHELP1(x) CCMGCCIGNOREDISABLEHELP0(GCC diagnostic warning x)
		#define CCMGCCIGNOREDISABLEHELP2(x) CCMGCCIGNOREDISABLEHELP1(#x)
	#endif
#endif

#ifndef CCM_DISABLE_IGNORE_GCC_WARNING
	#if defined(CCM_COMPILER_GCC) && (CCM_COMPILER_VERSION >= 4004)
		#define CCM_DISABLE_IGNORE_GCC_WARNING(w) _Pragma(CCMGCCIGNOREDISABLEHELP2(w)) _Pragma("GCC diagnostic pop")
	#else
		#define CCM_DISABLE_IGNORE_GCC_WARNING(w)
	#endif
#endif

// ------------------------------------------------------------------------
// CCM_DISABLE_CLANG_WARNING / CCM_RESTORE_CLANG_WARNING
//
// Example usage:
//     // Only one warning can be ignored per statement, due to how clang works.
//     CCM_DISABLE_CLANG_WARNING(-Wuninitialized)
//     CCM_DISABLE_CLANG_WARNING(-Wunused)
//     <code>
//     CCM_RESTORE_CLANG_WARNING()
//     CCM_RESTORE_CLANG_WARNING()
//
#ifndef CCM_DISABLE_CLANG_WARNING
	#if defined(CCM_COMPILER_CLANG_BASED) || defined(CCM_COMPILER_CLANG_CL)
		#define CCMCLANGWHELP0(x) #x
		#define CCMCLANGWHELP1(x) CCMCLANGWHELP0(clang diagnostic ignored x)
		#define CCMCLANGWHELP2(x) CCMCLANGWHELP1(#x)

		#define CCM_DISABLE_CLANG_WARNING(w) _Pragma("clang diagnostic push") _Pragma(CCMCLANGWHELP2(-Wunknown - warning - option)) _Pragma(CCMCLANGWHELP2(w))
	#else
		#define CCM_DISABLE_CLANG_WARNING(w)
	#endif
#endif

#ifndef CCM_RESTORE_CLANG_WARNING
	#if defined(CCM_COMPILER_CLANG_BASED) || defined(CCM_COMPILER_CLANG_CL)
		#define CCM_RESTORE_CLANG_WARNING() _Pragma("clang diagnostic pop")
	#else
		#define CCM_RESTORE_CLANG_WARNING()
	#endif
#endif

// ------------------------------------------------------------------------
// CCM_ENABLE_CLANG_WARNING_AS_ERROR / CCM_DISABLE_CLANG_WARNING_AS_ERROR
//
// Example usage:
//     // Only one warning can be treated as an error per statement, due to how clang works.
//     CCM_ENABLE_CLANG_WARNING_AS_ERROR(-Wuninitialized)
//     CCM_ENABLE_CLANG_WARNING_AS_ERROR(-Wunused)
//     <code>
//     CCM_DISABLE_CLANG_WARNING_AS_ERROR()
//     CCM_DISABLE_CLANG_WARNING_AS_ERROR()
//
#ifndef CCM_ENABLE_CLANG_WARNING_AS_ERROR
	#if defined(CCM_COMPILER_CLANG_BASED) || defined(CCM_COMPILER_CLANG_CL)
		#define CCMCLANGWERRORHELP0(x) #x
		#define CCMCLANGWERRORHELP1(x) CCMCLANGWERRORHELP0(clang diagnostic error x)
		#define CCMCLANGWERRORHELP2(x) CCMCLANGWERRORHELP1(#x)

		#define CCM_ENABLE_CLANG_WARNING_AS_ERROR(w) _Pragma("clang diagnostic push") _Pragma(CCMCLANGWERRORHELP2(w))
	#else
		#define CCM_DISABLE_CLANG_WARNING(w)
	#endif
#endif

#ifndef CCM_DISABLE_CLANG_WARNING_AS_ERROR
	#if defined(CCM_COMPILER_CLANG_BASED) || defined(CCM_COMPILER_CLANG_CL)
		#define CCM_DISABLE_CLANG_WARNING_AS_ERROR() _Pragma("clang diagnostic pop")
	#else
		#define CCM_DISABLE_CLANG_WARNING_AS_ERROR()
	#endif
#endif


// MSVC
#if defined(CCM_COMPILER_MSVC)
    #define CCM_DISABLE_MSVC_WARNING(w) __pragma(warning(push)) __pragma(warning(disable : w))
    #define CCM_RESTORE_MSVC_WARNING() __pragma(warning(pop))
    #define CCM_ENABLE_MSVC_WARNING_AS_ERROR(w) __pragma(warning(push)) __pragma(warning(error : w))
    #define CCM_DISABLE_MSVC_WARNING_AS_ERROR() __pragma(warning(pop))
#endif

