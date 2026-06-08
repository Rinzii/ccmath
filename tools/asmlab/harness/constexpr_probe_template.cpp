/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// asmlab constexpr probe template. constexpr_check.py substitutes @@TOKENS@@ and
// compiles this TU to verify the constexpr path without emitting runtime assembly.
// STATIC_ASSERTS block is substituted below with constexpr probe cases.

// clang-format off
@@INCLUDES@@
	// clang-format on

	@ @STATIC_ASSERTS @ @

	int main()
{ return 0; }
