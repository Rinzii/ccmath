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

#include <cstddef>
#include <cstdint>

// ModuleFn must be a function template: void ModuleFn<T>(uint8_t const*, size_t).
#define CCMATH_FUZZ_DRIVER(ModuleFn)                                                                                                                           \
	extern "C" int LLVMFuzzerTestOneInput(uint8_t const * data, size_t size)                                                                                   \
	{                                                                                                                                                          \
		ModuleFn<float>(data, size);                                                                                                                           \
		ModuleFn<double>(data, size);                                                                                                                          \
		return 0;                                                                                                                                              \
	}
