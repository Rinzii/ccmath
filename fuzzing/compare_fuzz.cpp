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
	void fuzz_compare_type(uint8_t const * data, size_t size)
	{
		if (size < 2 * sizeof(T)) { return; }

		T const x = ccm::fuzz::load_floating<T>(data, size, 0);
		T const y = ccm::fuzz::load_floating<T>(data, size, sizeof(T));

		FUZZ_CHECK(ccm::fpclassify(x) == std::fpclassify(x));
		FUZZ_CHECK(static_cast<bool>(ccm::isfinite(x)) == static_cast<bool>(std::isfinite(x)));
		FUZZ_CHECK(static_cast<bool>(ccm::isinf(x)) == static_cast<bool>(std::isinf(x)));
		FUZZ_CHECK(static_cast<bool>(ccm::isnan(x)) == static_cast<bool>(std::isnan(x)));
		FUZZ_CHECK(static_cast<bool>(ccm::isnormal(x)) == static_cast<bool>(std::isnormal(x)));
		FUZZ_CHECK(static_cast<bool>(ccm::signbit(x)) == static_cast<bool>(std::signbit(x)));

		ccm::fuzz::fuzz_binary_exact_int(x, y, ccm::isgreater<T>, [](T a, T b) { return std::isgreater(a, b); });
		ccm::fuzz::fuzz_binary_exact_int(x, y, ccm::isgreaterequal<T>, [](T a, T b) { return std::isgreaterequal(a, b); });
		ccm::fuzz::fuzz_binary_exact_int(x, y, ccm::isless<T>, [](T a, T b) { return std::isless(a, b); });
		ccm::fuzz::fuzz_binary_exact_int(x, y, ccm::islessequal<T>, [](T a, T b) { return std::islessequal(a, b); });
		ccm::fuzz::fuzz_binary_exact_int(x, y, ccm::islessgreater<T>, [](T a, T b) { return std::islessgreater(a, b); });
		ccm::fuzz::fuzz_binary_exact_int(x, y, ccm::isunordered<T>, [](T a, T b) { return std::isunordered(a, b); });
	}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(uint8_t const * data, size_t size)
{
	fuzz_compare_type<float>(data, size);
	fuzz_compare_type<double>(data, size);
	return 0;
}
