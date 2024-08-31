/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
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
