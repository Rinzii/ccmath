/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

// clang-format off


///
/// \def CCM_DISABLE_GCC_WARNING(w)
/// \brief Disables a warning for GCC.
/// \param w The warning to disable as an error. This should be written as -Wwarning-name and not as a string.
/// \note Only one warning can be disabled per statement, due to how gcc works.
/// \note This macro must be called before CCM_RESTORE_GCC_WARNING().
///
/// Example usage:
///     CCM_DISABLE_GCC_WARNING(-Wuninitialized)
///     CCM_DISABLE_GCC_WARNING(-Wunused)
///     <code>
///     CCM_RESTORE_GCC_WARNING()
///     CCM_RESTORE_GCC_WARNING()
///
#ifndef CCM_DISABLE_GCC_WARNING
	#if defined(__GNUC__) && !defined(__clang__)
        // Helper macros - do not use directly
		#define CCM_GCC_WHELP0(x) #x
		#define CCM_GCC_WHELP1(x) CCM_GCC_WHELP0(GCC diagnostic ignored x)
		#define CCM_GCC_WHELP2(x) CCM_GCC_WHELP1(#x)
																			 
		// Disables a warning for GCC. This should be written as -Wwarning-name and not as a string.
		// Must be called before CCM_RESTORE_GCC_WARNING().
		#define CCM_DISABLE_GCC_WARNING(w) _Pragma("GCC diagnostic push") _Pragma(CCM_GCC_WHELP2(w))
	#else
	// If not on GCC this macro does nothing.
	// Must be called before CCM_RESTORE_GCC_WARNING().
		#define CCM_DISABLE_GCC_WARNING(w)
	#endif
#endif

///
/// \def CCM_RESTORE_GCC_WARNING()
/// \brief Restore a warning for GCC. Warnings are restored in a stack based manner.
/// \note Only one warning can be disabled per statement, due to how gcc works.
/// \note This macro must be called after CCM_DISABLE_GCC_WARNING(w).
///
/// Example usage:
///     CCM_DISABLE_GCC_WARNING(-Wuninitialized)
///     CCM_DISABLE_GCC_WARNING(-Wunused)
///     <code>
///     CCM_RESTORE_GCC_WARNING()
///     CCM_RESTORE_GCC_WARNING()
///
#ifndef CCM_RESTORE_GCC_WARNING
	#if defined(__GNUC__) && !defined(__clang__)
		// Restore a warning for GCC in a stack based manner.
		// Must be called after CCM_DISABLE_GCC_WARNING(w).
		#define CCM_RESTORE_GCC_WARNING() _Pragma("GCC diagnostic pop")
	#else
	// If not on GCC this macro does nothing.
	// Must be called after CCM_DISABLE_GCC_WARNING(w).
		#define CCM_RESTORE_GCC_WARNING()
	#endif
#endif

///
/// \def CCM_ENABLE_GCC_WARNING_AS_ERROR(w)
/// \brief This will enable a warning as an error for GCC.
/// \param w The warning to enable as an error. This should be written as -Wwarning-name and not as a string.
/// \note Only one warning can be treated as an error per statement, due to how GCC works
/// \note This macro must be called before CCM_DISABLE_GCC_WARNING_AS_ERROR().
///
/// Example usage:
///     CCM_ENABLE_GCC_WARNING_AS_ERROR(-Wuninitialized)
///     CCM_ENABLE_GCC_WARNING_AS_ERROR(-Wunused)
///     <code>
///     CCM_DISABLE_GCC_WARNING_AS_ERROR()
///     CCM_DISABLE_GCC_WARNING_AS_ERROR()
///
#ifndef CCM_ENABLE_GCC_WARNING_AS_ERROR
	#if defined(__GNUC__) && !defined(__clang__)
		#define CCM_GCC_WERROR_HELP0(x) #x											   // Helper macros - do not use directly
		#define CCM_GCC_WERROR_HELP1(x) CCM_GCC_WERROR_HELP0(GCC diagnostic error x) // Helper macros - do not use directly
		#define CCM_GCC_WERROR_HELP2(x) CCM_GCC_WERROR_HELP1(#x)					   // Helper macros - do not use directly

		// This will enable a warning as an error for GCC. This should be written as -Wwarning-name and not as a string.
		// Must be called before CCM_DISABLE_GCC_WARNING_AS_ERROR().
		#define CCM_ENABLE_GCC_WARNING_AS_ERROR(w) _Pragma("GCC diagnostic push") _Pragma(CCM_GCC_WERROR_HELP2(w))
	#else
	// If not on GCC this macro does nothing.
	// Must be called before CCM_DISABLE_GCC_WARNING_AS_ERROR().
		#define CCM_ENABLE_GCC_WARNING_AS_ERROR(w)
	#endif
#endif

///
/// \def CCM_DISABLE_GCC_WARNING_AS_ERROR()
/// \brief This will disable a warning as an error for GCC.
/// \note Only one warning can be treated as an error per statement, due to how GCC works
///
/// Example usage:
///     CCM_ENABLE_GCC_WARNING_AS_ERROR(-Wuninitialized)
///     CCM_ENABLE_GCC_WARNING_AS_ERROR(-Wunused)
///     <code>
///     CCM_DISABLE_GCC_WARNING_AS_ERROR()
///     CCM_DISABLE_GCC_WARNING_AS_ERROR()
///
#ifndef CCM_DISABLE_GCC_WARNING_AS_ERROR
	#if defined(__GNUC__) && !defined(__clang__)
		// This will disable a warning as an error for GCC in a stack based manner.
		// Must be called after CCM_ENABLE_GCC_WARNING_AS_ERROR(w).
		#define CCM_DISABLE_GCC_WARNING_AS_ERROR() _Pragma("GCC diagnostic pop")
	#else
	// If not on GCC this macro does nothing.
	// Must be called after CCM_ENABLE_GCC_WARNING_AS_ERROR(w).
		#define CCM_DISABLE_GCC_WARNING_AS_ERROR()
	#endif
#endif

///
/// \def CCM_ENABLE_IGNORE_GCC_WARNING(w)
/// \brief This will enable the suppression of a warning on GCC.
/// \param w The warning to enable as an error. This should be written as -Wwarning-name and not as a string.
/// \note Only one warning can be treated as an error per statement, due to how GCC works
///
/// Example usage:
///     CCM_ENABLE_IGNORE_GCC_WARNING(-Wuninitialized)
///     CCM_ENABLE_IGNORE_GCC_WARNING(-Wunused)
///     <code>
///     CCM_DISABLE_IGNORE_GCC_WARNING(-Wuninitialized)
///     CCM_DISABLE_IGNORE_GCC_WARNING(-Wunused)
///
#ifndef CCM_ENABLE_IGNORE_GCC_WARNING
	#if defined(__GNUC__) && !defined(__clang__)
		#define CCM_GCC_IGNORE_HELP0(x) #x												 // Helper macros - do not use directly
		#define CCM_GCC_IGNORE_HELP1(x) CCM_GCC_IGNORE_HELP0(GCC diagnostic ignored x) // Helper macros - do not use directly
		#define CCM_GCC_IGNORE_HELP2(x) CCM_GCC_IGNORE_HELP1(#x)						 // Helper macros - do not use directly

		// This will enable the suppression of a warning on GCC. This should be written as -Wwarning-name and not as a string.
		// Must be called before CCM_DISABLE_IGNORE_GCC_WARNING(w).
		#define CCM_ENABLE_IGNORE_GCC_WARNING(w) _Pragma("GCC diagnostic push") _Pragma(CCM_GCC_IGNORE_HELP2(w))
	#else
	// If not on GCC this macro does nothing.
	// Must be called before CCM_DISABLE_IGNORE_GCC_WARNING(w).
		#define CCM_ENABLE_IGNORE_GCC_WARNING(w)
	#endif
#endif

///
/// \def CCM_DISABLE_IGNORE_GCC_WARNING(w)
/// \brief This will re-enable a ignored warning for GCC.
/// \param w The warning to re-enable as an warning. This should be written as -Wwarning-name and not as a string.
/// \note Only one warning can be treated as an error per statement, due to how GCC works
///
/// Example usage:
///     CCM_ENABLE_IGNORE_GCC_WARNING(-Wuninitialized)
///     CCM_ENABLE_IGNORE_GCC_WARNING(-Wunused)
///     <code>
///     CCM_DISABLE_IGNORE_GCC_WARNING(-Wuninitialized)
///     CCM_DISABLE_IGNORE_GCC_WARNING(-Wunused)
///
#ifndef CCM_DISABLE_IGNORE_GCC_WARNING
	#if defined(__GNUC__) && !defined(__clang__)
        #ifndef CCM_GCC_IGNORE_DISABLE_HELP0
		    #define CCM_GCC_IGNORE_DISABLE_HELP0(x) #x														 // Helper macros - do not use directly
        #endif
        #ifndef CCM_GCC_IGNORE_DISABLE_HELP1
            #define CCM_GCC_IGNORE_DISABLE_HELP1(x) CCM_GCC_IGNORE_DISABLE_HELP0(GCC diagnostic warning x) // Helper macros - do not use directly
        #endif
        #ifndef CCM_GCC_IGNORE_DISABLE_HELP2
            #define CCM_GCC_IGNORE_DISABLE_HELP2(x) CCM_GCC_IGNORE_DISABLE_HELP1(#x)						 // Helper macros - do not use directly
        #endif
		// This will disable suppression of an ignored warning for GCC
		// Must be called after CCM_ENABLE_IGNORE_GCC_WARNING(w).
		#define CCM_DISABLE_IGNORE_GCC_WARNING(w) _Pragma(CCM_GCC_IGNORE_DISABLE_HELP2(w)) _Pragma("GCC diagnostic pop")
	#else
	// If not on GCC this macro does nothing.
	// Must be called after CCM_ENABLE_IGNORE_GCC_WARNING(w).
		#define CCM_DISABLE_IGNORE_GCC_WARNING(w)
	#endif
#endif

///
/// \def CCM_DISABLE_CLANG_WARNING(w)
/// \brief Disables a warning for Clang.
/// \param w The warning to disable as an error. This should be written as -Wwarning-name and not as a string.
/// \note Only one warning can be disabled per statement, due to how clang works.
///
/// Example usage:
///     CCM_DISABLE_CLANG_WARNING(-Wuninitialized)
///     CCM_DISABLE_CLANG_WARNING(-Wunused)
///     <code>
///     CCM_RESTORE_CLANG_WARNING()
///     CCM_RESTORE_CLANG_WARNING()
///
#ifndef CCM_DISABLE_CLANG_WARNING
	#if defined(__clang__)
		#define CCM_CLANG_WHELP0(x) #x											   // Helper macros - do not use directly
		#define CCM_CLANG_WHELP1(x) CCM_CLANG_WHELP0(clang diagnostic ignored x) // Helper macros - do not use directly
		#define CCM_CLANG_WHELP2(x) CCM_CLANG_WHELP1(#x)						   // Helper macros - do not use directly

		// Disables a warning for Clang. This should be written as -Wwarning-name and not as a string.
		// Must be called before CCM_RESTORE_CLANG_WARNING().
		#define CCM_DISABLE_CLANG_WARNING(w)                                                                                                                  \
			_Pragma("clang diagnostic push") _Pragma(CCM_CLANG_WHELP2(-Wunknown-warning-option)) _Pragma(CCM_CLANG_WHELP2(w))
	#else
	// If not on Clang this macro does nothing.
	// Must be called before CCM_RESTORE_CLANG_WARNING().
		#define CCM_DISABLE_CLANG_WARNING(w)
	#endif
#endif

///
/// \def CCM_RESTORE_CLANG_WARNING()
/// \brief Restore a warning for Clang.
/// \param w The warning to disable as an error. This should be written as -Wwarning-name and not as a string.
/// \note Only one warning can be disabled per statement, due to how clang works.
///
/// Example usage:
///     CCM_DISABLE_CLANG_WARNING(-Wuninitialized)
///     CCM_DISABLE_CLANG_WARNING(-Wunused)
///     <code>
///     CCM_RESTORE_CLANG_WARNING()
///     CCM_RESTORE_CLANG_WARNING()
///
#ifndef CCM_RESTORE_CLANG_WARNING
	#if defined(__clang__)
		// Restore a warning for Clang in a stack based manner.
		// Must be called after CCM_DISABLE_CLANG_WARNING(w).
		#define CCM_RESTORE_CLANG_WARNING() _Pragma("clang diagnostic pop")
	#else
	// If not on Clang this macro does nothing.
	// Must be called after CCM_DISABLE_CLANG_WARNING(w).
		#define CCM_RESTORE_CLANG_WARNING()
	#endif
#endif

///
/// \def CCM_ENABLE_CLANG_WARNING_AS_ERROR(w)
/// \brief This will enable a warning as an error for clang.
/// \param w The warning to enable as an error. This should be written as -Wwarning-name and not as a string.
/// \note Only one warning can be treated as an error per statement, due to how clang works.
///
/// Example usage:
///     CCM_ENABLE_CLANG_WARNING_AS_ERROR(-Wuninitialized)
///     CCM_ENABLE_CLANG_WARNING_AS_ERROR(-Wunused)
///     <code>
///     CCM_DISABLE_CLANG_WARNING_AS_ERROR()
///     CCM_DISABLE_CLANG_WARNING_AS_ERROR()
///
#ifndef CCM_ENABLE_CLANG_WARNING_AS_ERROR
	#if defined(__clang__)
		// Helper macros
		#define CCM_CLANG_WERROR_HELP0(x) #x												 // Helper macros - do not use directly
		#define CCM_CLANG_WERROR_HELP1(x) CCM_CLANG_WERROR_HELP0(clang diagnostic error x) // Helper macros - do not use directly
		#define CCM_CLANG_WERROR_HELP2(x) CCM_CLANG_WERROR_HELP1(#x)						 // Helper macros - do not use directly

		// This will enable a warning as an error for clang. This should be written as -Wwarning-name and not as a string.
		// Must be called before CCM_DISABLE_CLANG_WARNING_AS_ERROR().
		#define CCM_ENABLE_CLANG_WARNING_AS_ERROR(w) _Pragma("clang diagnostic push") _Pragma(CCM_CLANG_WERROR_HELP2(w))
	#else
	// If not on clang this macro does nothing.
	// Must be called before CCM_DISABLE_CLANG_WARNING_AS_ERROR().
		#define CCM_ENABLE_CLANG_WARNING_AS_ERROR(w)
	#endif
#endif

///
/// \def CCM_DISABLE_CLANG_WARNING_AS_ERROR()
/// \brief This will disable a warning as an error for clang.
/// \note Only one warning can be treated as an error per statement, due to how clang works.
///
/// Example usage:
///     CCM_ENABLE_CLANG_WARNING_AS_ERROR(-Wuninitialized)
///     CCM_ENABLE_CLANG_WARNING_AS_ERROR(-Wunused)
///     <code>
///     CCM_DISABLE_CLANG_WARNING_AS_ERROR()
///     CCM_DISABLE_CLANG_WARNING_AS_ERROR()
///
#ifndef CCM_DISABLE_CLANG_WARNING_AS_ERROR
	#if defined(__clang__)
		// This will disable a warning as an error for clang in a stack based manner.
		// Must be called after CCM_ENABLE_CLANG_WARNING_AS_ERROR(w).
		#define CCM_DISABLE_CLANG_WARNING_AS_ERROR() _Pragma("clang diagnostic pop")
	#else
	// If not on clang this macro does nothing.
	// Must be called after CCM_ENABLE_CLANG_WARNING_AS_ERROR(w).
		#define CCM_DISABLE_CLANG_WARNING_AS_ERROR()
	#endif
#endif

///
/// \def CCM_DISABLE_MSVC_WARNING(w)
/// \brief Disables a warning for MSVC.
/// \param w The warning to disable as an error. Writen with a integer value to specify the warning.
/// \note This macro allows for multiple warnings to be disabled per statement by using a space between warnings.
/// \note This macro is effectively a wrapper for the VC++ #pragma warning(disable: warning-list) directive.
/// \note This macro must be called before CCM_RESTORE_VC_WARNING().
///
/// Example usage:
///     CCM_DISABLE_MSVC_WARNING(4100 4101)
///     <code>
///     CCM_RESTORE_MSVC_WARNING()
///
#ifndef CCM_DISABLE_MSVC_WARNING
	#if defined(_MSC_VER) && !defined(__clang__)
		// This will disable a list of warning for MSVC.
		// This macro allows for multiple warnings to be disabled per statement by using a space between warnings like so: CCM_DISABLE_MSVC_WARNING(4100 4101).
		// Must be called before CCM_RESTORE_MSVC_WARNING().
		#define CCM_DISABLE_VC_WARNING(w) __pragma(warning(push)) __pragma(warning(disable : w))
	#else
	// If not on MSVC this macro does nothing.
		#define CCM_DISABLE_VC_WARNING(w)
	#endif
#endif

///
/// \def CCM_RESTORE_MSVC_WARNING()
/// \brief Restore all disabled warnings for MSVC.
/// \note This macro must be called after CCM_DISABLE_VC_WARNING(w).
///
/// Example usage:
///     CCM_DISABLE_MSVC_WARNING(4100 4101)
///     <code>
///     CCM_RESTORE_MSVC_WARNING()
///
#ifndef CCM_RESTORE_MSVC_WARNING
	#if defined(_MSC_VER) && !defined(__clang__)
		// This will restore all disabled warnings for MSVC in a single statement.
		// Must be called after CCM_DISABLE_MSVC_WARNING(w).
		#define CCM_RESTORE_VC_WARNING() __pragma(warning(pop))
	#else
	// If not on MSVC this macro does nothing.
		#define CCM_RESTORE_VC_WARNING()
	#endif
#endif

///
/// \def CCM_ENABLE_MSVC_WARNING_AS_ERROR(w)
/// \brief This will enable a warning as an error for MSVC.
/// \param w The warning to enable as an error. Writen with a integer value to specify the warning.
/// \note This macro allows for multiple warnings to be disabled per statement by using a space between warnings.
/// \note This macro is effectively a wrapper for the VC++ #pragma warning(error: warning-list) directive.
/// \note This macro must be called before CCM_DISABLE_MSVC_WARNING_AS_ERROR().
///
/// Example usage:
///     CCM_ENABLE_MSVC_WARNING_AS_ERROR(4100 4101)
///     <code>
///     CCM_DISABLE_MSVC_WARNING_AS_ERROR()
///
#ifndef CCM_ENABLE_MSVC_WARNING_AS_ERROR
	#if defined(_MSC_VER) && !defined(__clang__)
		// This will enable a warning as an error for MSVC.
		// You can specify multiple warnings by using a space between warnings like so: CCM_ENABLE_MSVC_WARNING_AS_ERROR(4100 4101).
		// Must be called before CCM_DISABLE_MSVC_WARNING_AS_ERROR().
		#define CCM_ENABLE_MSVC_WARNING_AS_ERROR(w) __pragma(warning(push)) __pragma(warning(error : w))
	#else
	// If not on MSVC this macro does nothing.
		#define CCM_ENABLE_MSVC_WARNING_AS_ERROR(w)
	#endif
#endif

///
/// \def CCM_DISABLE_MSVC_WARNING_AS_ERROR()
/// \brief This will disable a warning as an error for MSVC.
/// \note This macro allows for multiple warnings to be disabled per statement by using a space between warnings.
/// \note This macro is effectively a wrapper for the VC++ #pragma warning(error: warning-list) directive.
/// \note This macro must be called after CCM_ENABLE_MSVC_WARNING_AS_ERROR(w).
///
/// Example usage:
///     CCM_ENABLE_MSVC_WARNING_AS_ERROR(4100 4101)
///     <code>
///     CCM_DISABLE_MSVC_WARNING_AS_ERROR()
///
#ifndef CCM_DISABLE_MSVC_WARNING_AS_ERROR
	#if defined(_MSC_VER) && !defined(__clang__)
		// This will restore all disabled warnings for MSVC in a single statement.
		// Must be called after CCM_ENABLE_MSVC_WARNING_AS_ERROR(w).
		#define CCM_DISABLE_MSVC_WARNING_AS_ERROR(w) __pragma(warning(pop))
	#else
	// If not on MSVC this macro does nothing.
	// Must be called after CCM_ENABLE_MSVC_WARNING_AS_ERROR(w).
		#define CCM_DISABLE_MSVC_WARNING_AS_ERROR(w)
	#endif
#endif

///
/// \def CCM_DISABLE_ALL_MSVC_WARNINGS()
/// \brief This will disable all warnings for MSVC.
/// \note This macro must be called before CCM_RESTORE_ALL_MSVC_WARNINGS().
///
/// Example usage:
///     CCM_DISABLE_ALL_MSVC_WARNINGS()
///     <code>
///     CCM_RESTORE_ALL_MSVC_WARNINGS()
///
#ifndef CCM_DISABLE_ALL_MSVC_WARNINGS
	#if defined(_MSC_VER) && !defined(__clang__)
		// This will disable all warnings for MSVC.
		#define CCM_DISABLE_ALL_MSVC_WARNINGS()                                                                                                               \
			__pragma(warning(push, 0)) __pragma(warning(                                                                                                       \
				disable : 4244 4265 4267 4350 4472 4509 4548 4623 4710 4985 6320 4755 4625 4626 4702)) // Some warnings have to be explicitly specified.
	#else
	// If not on MSVC this macro does nothing.
		#define CCM_DISABLE_ALL_MSVC_WARNINGS()
	#endif
#endif

///
/// \def CCM_RESTORE_ALL_MSVC_WARNINGS()
/// \brief This will restore all disabled warnings for MSVC.
/// \note This macro must be called after CCM_DISABLE_ALL_MSVC_WARNINGS().
///
/// Example usage:
///     CCM_DISABLE_ALL_MSVC_WARNINGS()
///     <code>
///     CCM_RESTORE_ALL_MSVC_WARNINGS()
///
#ifndef CCM_RESTORE_ALL_MSVC_WARNINGS
	#if defined(_MSC_VER) && !defined(__clang__)
		// This will restore all disabled warnings for MSVC in a single statement.
		#define CCM_RESTORE_ALL_MSVC_WARNINGS() __pragma(warning(pop))
	#else
	// If not on MSVC this macro does nothing.
		#define CCM_RESTORE_ALL_MSVC_WARNINGS()
	#endif
#endif

// clang-format on
