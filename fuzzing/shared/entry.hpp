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

#include "harness.hpp"

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

// Adds a long double lane. Only for modules whose long double overloads are exact bit
// operations or native kernels: delegating overloads are correct by policy but diverge from
// the native long double libm by far more than the harness ulp bound.
#define CCMATH_FUZZ_DRIVER_ALL_TYPES(ModuleFn)                                                                                                                 \
	extern "C" int LLVMFuzzerTestOneInput(uint8_t const * data, size_t size)                                                                                   \
	{                                                                                                                                                          \
		ModuleFn<float>(data, size);                                                                                                                           \
		ModuleFn<double>(data, size);                                                                                                                          \
		ModuleFn<long double>(data, size);                                                                                                                     \
		return 0;                                                                                                                                              \
	}
