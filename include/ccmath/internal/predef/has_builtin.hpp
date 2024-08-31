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

#ifndef CCM_HAS_BUILTIN
	#undef CCM_HAS_BUILTIN
#endif // CCM_HAS_BUILTIN

#if defined(__has_builtin)
	#define CCM_HAS_BUILTIN(builtin) __has_builtin(builtin)
#else
	#define CCM_HAS_BUILTIN(builtin) (0)
#endif
