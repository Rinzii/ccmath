/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// asmlab harness template. emit.py substitutes the @@TOKENS@@ from the function
// registry and compiles the result to assembly. This file is never compiled as-is.
//
// Why this works: the target is an extern "C", non-inline wrapper whose arguments
// arrive as ordinary runtime parameters. External linkage forces the body to be
// emitted as one stable symbol named @@SYMBOL@@, so it cannot be constant-folded
// or dead-stripped, and there is no main() and no <cmath>. The compile-time gen
// path is therefore bypassed and we observe exactly the runtime code generation.
//
// Volatile loads block manifest constant evaluation of the call arguments.
// @@FLATTEN_COMMENT@@

// clang-format off
@@INCLUDES@@
// clang-format on

#if defined(__GNUC__) || defined(__clang__)
	#define ASMLAB_FLATTEN @ @FLATTEN_ATTR @ @
#else
	#define ASMLAB_FLATTEN
#endif

	extern "C" volatile unsigned long long asmlab_opaque_seed;

extern "C" ASMLAB_FLATTEN @ @RET @ @ @ @SYMBOL @ @(@ @PARAMS @ @)
{ @ @VOLATILE_LOADS @ @ return @ @EXPR @ @; }
