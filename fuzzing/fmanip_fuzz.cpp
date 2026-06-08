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
	void fuzz_fmanip_type(uint8_t const * data, size_t size)
	{
		if (size < 2 * sizeof(T) + sizeof(int32_t)) { return; }

		T const x	= ccm::fuzz::load_floating<T>(data, size, 0);
		T const y	= ccm::fuzz::load_floating<T>(data, size, sizeof(T));
		int const n = static_cast<int>(ccm::fuzz::load_i32(data, size, 2 * sizeof(T)));

		ccm::fuzz::fuzz_binary_vs_std(x, y, ccm::copysign<T>, [](T a, T b) { return std::copysign(a, b); });
		ccm::fuzz::fuzz_binary_vs_std(x, y, ccm::nextafter<T>, [](T a, T b) { return std::nextafter(a, b); });
		long double const toward = static_cast<long double>(y);
		ccm::fuzz::check_same_floating(ccm::nexttoward(x, toward), std::nexttoward(x, toward));
		ccm::fuzz::check_same_floating(ccm::scalbn(x, n), std::scalbn(x, n));
		ccm::fuzz::check_same_floating(ccm::ldexp(x, n), std::ldexp(x, n));

		ccm::fuzz::check_frexp_vs_std(x);
		ccm::fuzz::check_modf_vs_std(x);

		FUZZ_CHECK(ccm::ilogb(x) == std::ilogb(x));
		ccm::fuzz::check_same_floating(ccm::logb(x), std::logb(x));
	}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(uint8_t const * data, size_t size)
{
	fuzz_fmanip_type<float>(data, size);
	fuzz_fmanip_type<double>(data, size);
	return 0;
}
