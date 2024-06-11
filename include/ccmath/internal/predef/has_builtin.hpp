/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#ifndef CCM_HAS_BUILTIN
	#undef CCM_HAS_BUILTIN
#endif // CCM_HAS_BUILTIN

#if defined(__has_builtin)
	#define CCM_HAS_BUILTIN(builtin) __has_builtin(builtin)
#else
	#define CCM_HAS_BUILTIN(builtin) (0)
#endif
