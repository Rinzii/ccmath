/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

// TODO: This should work fine on clang based compilers, but I still need to validate this is fact.
// TODO: I need to validate that clang-cl works with this as well on windows.

///
/// \def CCM_DISABLE_CLANG_WARNING(w)
/// \brief Disables a warning for Clang.
/// \param w The warning to disable as an error. This should be written as -Wwarning-name and not as a string.
/// \note Only one warning can be disabled per statement, due to how clang works.
///
/// WARNING: You have to disable clang-format temporarily when passing flags
///			 to this function as clang-format likes to put spaces between the hyphens in flags
///          like the following: -Wunknown - warning - option
///
/// WARNING EXAMPLE:
///	 // clang-format off
///	 CCM_DISABLE_CLANG_WARNING(-Wuninitialized)
///	 // clang-format on
///  <code>
///	 CCM_RESTORE_CLANG_WARNING()
///
///
/// Example usage of macro:
///	    // clang-format off
///     CCM_DISABLE_CLANG_WARNING(-Wuninitialized)
///     CCM_DISABLE_CLANG_WARNING(-Wunused)
///	    // clang-format on
///     <code>
///     CCM_RESTORE_CLANG_WARNING()
///     CCM_RESTORE_CLANG_WARNING()
///
#ifndef CCM_DISABLE_CLANG_WARNING
	#if defined(__clang__)
	// Disable clang format so it does not format "-Wunknown-warning-option" to "- Wunknown - Wwarning - Woption"
	// clang-format off
		#define CCM_CLANG_WHELP0(x) #x											 // Helper macros - do not use directly
		#define CCM_CLANG_WHELP1(x) CCM_CLANG_WHELP0(clang diagnostic ignored x) // Helper macros - do not use directly
		#define CCM_CLANG_WHELP2(x) CCM_CLANG_WHELP1(#x)						 // Helper macros - do not use directly

		// Disables a warning for Clang. This should be written as -Wwarning-name and not as a string.
		// Must be called before CCM_RESTORE_CLANG_WARNING().
		#define CCM_DISABLE_CLANG_WARNING(w)                                                                                                                  \
			_Pragma("clang diagnostic push") _Pragma(CCM_CLANG_WHELP2(-Wunknown-warning-option)) _Pragma(CCM_CLANG_WHELP2(w))
	// clang-format on
#else
	// If not on Clang, this macro does nothing.
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
/// Example usage of macro:
///	    // clang-format off
///     CCM_DISABLE_CLANG_WARNING(-Wuninitialized)
///     CCM_DISABLE_CLANG_WARNING(-Wunused)
///	    // clang-format on
///     <code>
///     CCM_RESTORE_CLANG_WARNING()
///     CCM_RESTORE_CLANG_WARNING()
///
#ifndef CCM_RESTORE_CLANG_WARNING
	#if defined(__clang__)
		// Restore a warning for Clang in a stack-based manner.
		// Must be called after CCM_DISABLE_CLANG_WARNING(w).
		#define CCM_RESTORE_CLANG_WARNING() _Pragma("clang diagnostic pop")
	#else
	// If not on Clang, this macro does nothing.
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
/// WARNING: You have to disable clang-format temporarily when passing flags
///			 to this function as clang-format likes to put spaces between the hyphens in flags
///          like the following: -Wunknown - warning - option
///
/// WARNING EXAMPLE:
///	 // clang-format off
///	 CCM_ENABLE_CLANG_WARNING_AS_ERROR(-Wuninitialized)
///	 // clang-format on
///  <code>
///	 CCM_DISABLE_CLANG_WARNING_AS_ERROR()
///
///
/// Example usage of macro:
///	    // clang-format off
///     CCM_ENABLE_CLANG_WARNING_AS_ERROR(-Wuninitialized)
///     CCM_ENABLE_CLANG_WARNING_AS_ERROR(-Wunused)
///	    // clang-format on
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
	// If not on clang, this macro does nothing.
	// Must be called before CCM_DISABLE_CLANG_WARNING_AS_ERROR().
		#define CCM_ENABLE_CLANG_WARNING_AS_ERROR(w)
	#endif
#endif

///
/// \def CCM_DISABLE_CLANG_WARNING_AS_ERROR()
/// \brief This will disable a warning as an error for clang.
/// \note Only one warning can be treated as an error per statement, due to how clang works.
///
/// Example usage of macro:
///	    // clang-format off
///     CCM_ENABLE_CLANG_WARNING_AS_ERROR(-Wuninitialized)
///     CCM_ENABLE_CLANG_WARNING_AS_ERROR(-Wunused)
///	    // clang-format on
///     <code>
///     CCM_DISABLE_CLANG_WARNING_AS_ERROR()
///     CCM_DISABLE_CLANG_WARNING_AS_ERROR()
///
#ifndef CCM_DISABLE_CLANG_WARNING_AS_ERROR
	#if defined(__clang__)
		// This will disable a warning as an error for clang in a stack-based manner.
		// Must be called after CCM_ENABLE_CLANG_WARNING_AS_ERROR(w).
		#define CCM_DISABLE_CLANG_WARNING_AS_ERROR() _Pragma("clang diagnostic pop")
	#else
	// If not on clang, this macro does nothing.
	// Must be called after CCM_ENABLE_CLANG_WARNING_AS_ERROR(w).
		#define CCM_DISABLE_CLANG_WARNING_AS_ERROR()
	#endif
#endif