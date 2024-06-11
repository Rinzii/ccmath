/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

// TODO: This should work fine on gcc based compilers, but I still need to validate this is fact.

///
/// \def CCM_DISABLE_GCC_WARNING(w)
/// \brief Disables a warning for GCC.
/// \param w The warning to disable as an error. This should be written as -Wwarning-name and not as a string.
/// \note Only one warning can be disabled per statement, due to how gcc works.
/// \note This macro must be called before CCM_RESTORE_GCC_WARNING().
///
/// WARNING: You have to disable clang-format temporarily when passing flags
///			 to this function as clang-format likes to put spaces between the hyphens in flags
///          like the following: -Wunknown - warning - option
///
/// WARNING EXAMPLE:
///	 // clang-format off
///	 CCM_DISABLE_GCC_WARNING(-Wunknown-warning-option)
///	 // clang-format on
///  <code>
///	 CCM_RESTORE_GCC_WARNING()
///
///
/// Example usage of macro:
///	    // clang-format off
///     CCM_DISABLE_GCC_WARNING(-Wuninitialized)
///     CCM_DISABLE_GCC_WARNING(-Wunused)
///	    // clang-format on
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
	// If not on GCC, this macro does nothing.
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
/// Example usage of macro:
///	    // clang-format off
///     CCM_DISABLE_GCC_WARNING(-Wuninitialized)
///     CCM_DISABLE_GCC_WARNING(-Wunused)
///	    // clang-format on
///     <code>
///     CCM_RESTORE_GCC_WARNING()
///     CCM_RESTORE_GCC_WARNING()
///
#ifndef CCM_RESTORE_GCC_WARNING
	#if defined(__GNUC__) && !defined(__clang__)
		// Restore a warning for GCC in a stack-based manner.
		// Must be called after CCM_DISABLE_GCC_WARNING(w).
		#define CCM_RESTORE_GCC_WARNING() _Pragma("GCC diagnostic pop")
	#else
	// If not on GCC, this macro does nothing.
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
/// WARNING: You have to disable clang-format temporarily when passing flags
///			 to this function as clang-format likes to put spaces between the hyphens in flags
///          like the following: -Wunknown - warning - option
///
/// WARNING EXAMPLE:
///	 // clang-format off
///	 CCM_ENABLE_GCC_WARNING_AS_ERROR(-Wunknown-warning-option)
///	 // clang-format on
///  <code>
///	 CCM_DISABLE_GCC_WARNING_AS_ERROR()
///
///
/// Example usage of macro:
///	    // clang-format off
///     CCM_ENABLE_GCC_WARNING_AS_ERROR(-Wuninitialized)
///     CCM_ENABLE_GCC_WARNING_AS_ERROR(-Wunused)
///	    // clang-format on
///     <code>
///     CCM_DISABLE_GCC_WARNING_AS_ERROR()
///     CCM_DISABLE_GCC_WARNING_AS_ERROR()
///
#ifndef CCM_ENABLE_GCC_WARNING_AS_ERROR
	#if defined(__GNUC__) && !defined(__clang__)
		#define CCM_GCC_WERROR_HELP0(x) #x											 // Helper macros - do not use directly
		#define CCM_GCC_WERROR_HELP1(x) CCM_GCC_WERROR_HELP0(GCC diagnostic error x) // Helper macros - do not use directly
		#define CCM_GCC_WERROR_HELP2(x) CCM_GCC_WERROR_HELP1(#x)					 // Helper macros - do not use directly

		// This will enable a warning as an error for GCC. This should be written as -Wwarning-name and not as a string.
		// Must be called before CCM_DISABLE_GCC_WARNING_AS_ERROR().
		#define CCM_ENABLE_GCC_WARNING_AS_ERROR(w) _Pragma("GCC diagnostic push") _Pragma(CCM_GCC_WERROR_HELP2(w))
	#else
	// If not on GCC, this macro does nothing.
	// Must be called before CCM_DISABLE_GCC_WARNING_AS_ERROR().
		#define CCM_ENABLE_GCC_WARNING_AS_ERROR(w)
	#endif
#endif

///
/// \def CCM_DISABLE_GCC_WARNING_AS_ERROR()
/// \brief This will disable a warning as an error for GCC.
/// \note Only one warning can be treated as an error per statement, due to how GCC works
///
/// Example usage of macro:
///	    // clang-format off
///     CCM_ENABLE_GCC_WARNING_AS_ERROR(-Wuninitialized)
///     CCM_ENABLE_GCC_WARNING_AS_ERROR(-Wunused)
///	    // clang-format on
///     <code>
///     CCM_DISABLE_GCC_WARNING_AS_ERROR()
///     CCM_DISABLE_GCC_WARNING_AS_ERROR()
///
#ifndef CCM_DISABLE_GCC_WARNING_AS_ERROR
	#if defined(__GNUC__) && !defined(__clang__)
		// This will disable a warning as an error for GCC in a stack-based manner.
		// Must be called after CCM_ENABLE_GCC_WARNING_AS_ERROR(w).
		#define CCM_DISABLE_GCC_WARNING_AS_ERROR() _Pragma("GCC diagnostic pop")
	#else
	// If not on GCC, this macro does nothing.
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
/// WARNING: You have to disable clang-format temporarily when passing flags
///			 to this function as clang-format likes to put spaces between the hyphens in flags
///          like the following: -Wunknown - warning - option
///
/// WARNING EXAMPLE:
///	 // clang-format off
///	 CCM_ENABLE_IGNORE_GCC_WARNING(-Wuninitialized)
///	 // clang-format on
///  <code>
///	 // clang-format off
///	 CCM_DISABLE_IGNORE_GCC_WARNING(-Wuninitialized)
///	 // clang-format on
///
///
/// Example usage of macro:
///	    // clang-format off
///     CCM_ENABLE_IGNORE_GCC_WARNING(-Wuninitialized)
///     CCM_ENABLE_IGNORE_GCC_WARNING(-Wunused)
///	    // clang-format on
///     <code>
///	    // clang-format off
///     CCM_DISABLE_IGNORE_GCC_WARNING(-Wuninitialized)
///     CCM_DISABLE_IGNORE_GCC_WARNING(-Wunused)
///	 // clang-format on
///
#ifndef CCM_ENABLE_IGNORE_GCC_WARNING
	#if defined(__GNUC__) && !defined(__clang__)
		#define CCM_GCC_IGNORE_HELP0(x) #x											   // Helper macros - do not use directly
		#define CCM_GCC_IGNORE_HELP1(x) CCM_GCC_IGNORE_HELP0(GCC diagnostic ignored x) // Helper macros - do not use directly
		#define CCM_GCC_IGNORE_HELP2(x) CCM_GCC_IGNORE_HELP1(#x)					   // Helper macros - do not use directly

		// This will enable the suppression of a warning on GCC. This should be written as -Wwarning-name and not as a string.
		// Must be called before CCM_DISABLE_IGNORE_GCC_WARNING(w).
		#define CCM_ENABLE_IGNORE_GCC_WARNING(w) _Pragma("GCC diagnostic push") _Pragma(CCM_GCC_IGNORE_HELP2(w))
	#else
	// If not on GCC, this macro does nothing.
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
/// Example usage of macro:
///	    // clang-format off
///     CCM_ENABLE_IGNORE_GCC_WARNING(-Wuninitialized)
///     CCM_ENABLE_IGNORE_GCC_WARNING(-Wunused)
///     <code>
///     CCM_DISABLE_IGNORE_GCC_WARNING(-Wuninitialized)
///     CCM_DISABLE_IGNORE_GCC_WARNING(-Wunused)
///
#ifndef CCM_DISABLE_IGNORE_GCC_WARNING
	#if defined(__GNUC__) && !defined(__clang__)
		#ifndef CCM_GCC_IGNORE_DISABLE_HELP0
			#define CCM_GCC_IGNORE_DISABLE_HELP0(x) #x // Helper macros - do not use directly
		#endif
		#ifndef CCM_GCC_IGNORE_DISABLE_HELP1
			#define CCM_GCC_IGNORE_DISABLE_HELP1(x) CCM_GCC_IGNORE_DISABLE_HELP0(GCC diagnostic warning x) // Helper macros - do not use directly
		#endif
		#ifndef CCM_GCC_IGNORE_DISABLE_HELP2
			#define CCM_GCC_IGNORE_DISABLE_HELP2(x) CCM_GCC_IGNORE_DISABLE_HELP1(#x) // Helper macros - do not use directly
		#endif
		// This will disable suppression of an ignored warning for GCC
		// Must be called after CCM_ENABLE_IGNORE_GCC_WARNING(w).
		#define CCM_DISABLE_IGNORE_GCC_WARNING(w) _Pragma(CCM_GCC_IGNORE_DISABLE_HELP2(w)) _Pragma("GCC diagnostic pop")
	#else
	// If not on GCC, this macro does nothing.
	// Must be called after CCM_ENABLE_IGNORE_GCC_WARNING(w).
		#define CCM_DISABLE_IGNORE_GCC_WARNING(w)
	#endif
#endif