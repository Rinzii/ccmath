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

#if defined(__GNUC__) || (defined(__clang__) && !defined(_MSC_VER))
	#define CCM_TRAP() __builtin_trap() // This is not resumable as it ends the basic block and the optimizer will strip all following code as dead
#else									// fallback to abort if we are not on GCC or Clang
extern "C" [[noreturn]] void abort();
	#define CCM_TRAP() abort();
#endif
