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
	T reference_lerp(T a, T b, T t)
	{
		if (std::isnan(a) || std::isnan(b) || std::isnan(t)) { return a + b + t; }

		if ((a <= T{} && b >= T{}) || (a >= T{} && b <= T{})) { return t * b + (static_cast<T>(1) - t) * a; }
		if (t == static_cast<T>(1)) { return b; }

		T const x = a + t * (b - a);
		if ((t > static_cast<T>(1)) == (b > a)) { return b < x ? x : b; }
		return x < b ? x : b;
	}

	template <typename T>
	void fuzz_misc_type(uint8_t const * data, size_t size)
	{
		if (size < 3 * sizeof(T)) { return; }

		T const a = ccm::fuzz::load_floating<T>(data, size, 0);
		T const b = ccm::fuzz::load_floating<T>(data, size, sizeof(T));
		T const t = ccm::fuzz::load_floating<T>(data, size, 2 * sizeof(T));

		ccm::fuzz::check_same_floating(ccm::lerp(a, b, t), reference_lerp(a, b, t));

		if (a < T{} && a == std::trunc(a)) { return; }
		ccm::fuzz::fuzz_unary_vs_std(a, ccm::gamma<T>, [](T v) { return std::tgamma(v); });
		ccm::fuzz::fuzz_unary_vs_std(a, ccm::lgamma<T>, [](T v) { return std::lgamma(v); });
	}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(uint8_t const * data, size_t size)
{
	fuzz_misc_type<float>(data, size);
	fuzz_misc_type<double>(data, size);
	return 0;
}
