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
	void fuzz_nearest_type(uint8_t const * data, size_t size)
	{
		if (size < sizeof(T)) { return; }

		T const x = ccm::fuzz::load_floating<T>(data, size, 0);

		ccm::fuzz::fuzz_unary_vs_std(x, ccm::floor<T>, [](T v) { return std::floor(v); });
		ccm::fuzz::fuzz_unary_vs_std(x, ccm::trunc<T>, [](T v) { return std::trunc(v); });
		ccm::fuzz::fuzz_unary_vs_std(x, ccm::nearbyint<T>, [](T v) { return std::nearbyint(v); });
		ccm::fuzz::fuzz_unary_vs_std(x, ccm::ceil<T>, [](T v) { return std::ceil(v); });
		ccm::fuzz::fuzz_unary_vs_std(x, ccm::round<T>, [](T v) { return std::round(v); });
		ccm::fuzz::fuzz_unary_vs_std(x, ccm::rint<T>, [](T v) { return std::rint(v); });
	}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(uint8_t const * data, size_t size)
{
	fuzz_nearest_type<float>(data, size);
	fuzz_nearest_type<double>(data, size);
	return 0;
}
