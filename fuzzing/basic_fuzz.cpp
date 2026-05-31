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
	void fuzz_basic_type(uint8_t const * data, size_t size)
	{
		if (size < 3 * sizeof(T)) { return; }

		T const x = ccm::fuzz::load_floating<T>(data, size, 0);
		T const y = ccm::fuzz::load_floating<T>(data, size, sizeof(T));
		T const z = ccm::fuzz::load_floating<T>(data, size, 2 * sizeof(T));

		ccm::fuzz::fuzz_unary_vs_std(x, ccm::fabs<T>, [](T v) { return std::fabs(v); });
		ccm::fuzz::fuzz_binary_vs_std(x, y, ccm::fdim<T>, [](T a, T b) { return std::fdim(a, b); });
		ccm::fuzz::check_same_floating(ccm::fma(x, y, z), std::fma(x, y, z));
		ccm::fuzz::fuzz_binary_vs_std(x, y, ccm::fmod<T>, [](T a, T b) { return std::fmod(a, b); });
		ccm::fuzz::fuzz_binary_vs_std(x, y, ccm::max<T>, [](T a, T b) { return std::fmax(a, b); });
		ccm::fuzz::fuzz_binary_vs_std(x, y, ccm::min<T>, [](T a, T b) { return std::fmin(a, b); });
		ccm::fuzz::fuzz_binary_vs_std(x, y, ccm::remainder<T>, [](T a, T b) { return std::remainder(a, b); });

		ccm::fuzz::check_remquo_vs_std(x, y);
	}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(uint8_t const * data, size_t size)
{
	fuzz_basic_type<float>(data, size);
	fuzz_basic_type<double>(data, size);
	return 0;
}
