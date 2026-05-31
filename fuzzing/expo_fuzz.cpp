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
	void fuzz_expo_type(uint8_t const * data, size_t size)
	{
		if (size < sizeof(T)) { return; }

		T const x = ccm::fuzz::load_floating<T>(data, size, 0);

		ccm::fuzz::fuzz_unary_vs_std(x, ccm::exp<T>, [](T v) { return std::exp(v); });
		ccm::fuzz::fuzz_unary_vs_std(x, ccm::exp2<T>, [](T v) { return std::exp2(v); });
		ccm::fuzz::fuzz_unary_vs_std(x, ccm::expm1<T>, [](T v) { return std::expm1(v); });
		ccm::fuzz::fuzz_unary_log_family(x, ccm::log<T>, [](T v) { return std::log(v); });
		ccm::fuzz::fuzz_unary_log1p_family(x, ccm::log1p<T>, [](T v) { return std::log1p(v); });
		ccm::fuzz::fuzz_unary_log_family(x, ccm::log2<T>, [](T v) { return std::log2(v); });
		ccm::fuzz::fuzz_unary_log_family(x, ccm::log10<T>, [](T v) { return std::log10(v); });
	}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(uint8_t const * data, size_t size)
{
	fuzz_expo_type<float>(data, size);
	fuzz_expo_type<double>(data, size);
	return 0;
}
