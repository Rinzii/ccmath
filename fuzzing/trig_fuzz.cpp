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
	void fuzz_trig_type(uint8_t const * data, size_t size)
	{
		if (size < 2 * sizeof(T)) { return; }

		T const x = ccm::fuzz::load_floating<T>(data, size, 0);
		T const y = ccm::fuzz::load_floating<T>(data, size, sizeof(T));

		ccm::fuzz::fuzz_unary_vs_std(x, ccm::sin<T>, [](T v) { return std::sin(v); });
		ccm::fuzz::fuzz_unary_vs_std(x, ccm::cos<T>, [](T v) { return std::cos(v); });
		ccm::fuzz::fuzz_unary_vs_std(x, ccm::tan<T>, [](T v) { return std::tan(v); });
		ccm::fuzz::fuzz_unary_vs_std(x, ccm::asin<T>, [](T v) { return std::asin(v); });
		ccm::fuzz::fuzz_unary_vs_std(x, ccm::acos<T>, [](T v) { return std::acos(v); });
		ccm::fuzz::fuzz_unary_vs_std(x, ccm::atan<T>, [](T v) { return std::atan(v); });
		ccm::fuzz::fuzz_binary_vs_std(x, y, ccm::atan2<T>, [](T a, T b) { return std::atan2(a, b); });
	}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(uint8_t const * data, size_t size)
{
	fuzz_trig_type<float>(data, size);
	fuzz_trig_type<double>(data, size);
	return 0;
}
