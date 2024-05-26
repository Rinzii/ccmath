/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

// This is a clang extension that allows for checking if a builtin is available at compile-time.
#ifndef CCM_HAS_CONST_BUILTIN
	#if defined(__has_constexpr_builtin)
		#define CCM_HAS_CONST_BUILTIN(BUILTIN) __has_constexpr_builtin(BUILTIN)
	#else
		#define CCM_HAS_CONST_BUILTIN(BUILTIN) 0
	#endif // defined(__has_constexpr_builtin)
#endif // CCM_HAS_CONST_BUILTIN
