/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

// TODO: Need to validate these macros have no issue when clang-cl is installed alongside MSVC. My assumption is it's fine though at the moment.

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
		#define CCM_DISABLE_MSVC_WARNING(w) __pragma(warning(push)) __pragma(warning(disable : w))
	#else
	// If not on MSVC, this macro does nothing.
		#define CCM_DISABLE_MSVC_WARNING(w)
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
		#define CCM_RESTORE_MSVC_WARNING() __pragma(warning(pop))
	#else
	// If not on MSVC, this macro does nothing.
		#define CCM_RESTORE_MSVC_WARNING()
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
	// If not on MSVC, this macro does nothing.
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
	// If not on MSVC, this macro does nothing.
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
		#define CCM_DISABLE_ALL_MSVC_WARNINGS()                                                                                                                \
			__pragma(warning(push, 0)) __pragma(warning(                                                                                                       \
				disable : 4244 4265 4267 4350 4472 4509 4548 4623 4710 4985 6320 4755 4625 4626 4702)) // Some warnings have to be explicitly specified.
	#else
	// If not on MSVC, this macro does nothing.
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
	// If not on MSVC, this macro does nothing.
		#define CCM_RESTORE_ALL_MSVC_WARNINGS()
	#endif
#endif