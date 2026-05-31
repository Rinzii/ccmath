/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "harness.hpp"

#include <ccmath/ccmath.hpp>
#include <cmath>
#include <cstdint>

namespace
{
	template <typename T>
	void fuzz_power_type(uint8_t const * data, size_t size)
	{
		if (size < 2 * sizeof(T)) { return; }

		T const x = ccm::fuzz::load_floating<T>(data, size, 0);
		T const y = ccm::fuzz::load_floating<T>(data, size, sizeof(T));

		ccm::fuzz::fuzz_unary_vs_std(x, ccm::sqrt<T>, [](T v) { return std::sqrt(v); });
		ccm::fuzz::fuzz_unary_vs_std(x, ccm::cbrt<T>, [](T v) { return std::cbrt(v); });
		ccm::fuzz::fuzz_binary_vs_std(x, y, ccm::pow<T>, [](T a, T b) { return std::pow(a, b); });
		ccm::fuzz::fuzz_binary_vs_std(x, y, ccm::hypot<T>, [](T a, T b) { return std::hypot(a, b); });
	}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(uint8_t const * data, size_t size)
{
	fuzz_power_type<float>(data, size);
	fuzz_power_type<double>(data, size);
	return 0;
}
