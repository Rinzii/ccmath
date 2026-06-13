/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "entry.hpp"
#include "fuzz_calls.hpp"
#include "harness.hpp"

#include <ccmath/ccmath.hpp>

#include <cmath>
#include <limits>

namespace
{
	template <typename T>
	void power(uint8_t const * data, size_t size)
	{
		ccm::fuzz::Inputs<T> in;
		if (!in.load_xy(data, size)) { return; }

		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::fuzz::calls::sqrt<T>, [](T v) { return std::sqrt(v); });
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::fuzz::calls::cbrt<T>, [](T v) { return std::cbrt(v); });

		// TODO(IanP): drop the overflow-region guard once hypot rounds up to inf at the
		// overflow boundary instead of returning the largest finite value (chip).
		const T hypot_overflow_edge = std::numeric_limits<T>::max() * static_cast<T>(0.5);
		const bool hypot_overflow_region =
			std::isfinite(in.x) && std::isfinite(in.y) && (std::fabs(in.x) > hypot_overflow_edge || std::fabs(in.y) > hypot_overflow_edge);
#ifdef CCMATH_FUZZ_GENERIC
		const bool skip_hypot = hypot_overflow_region;
#else
		const bool skip_hypot = false;
#endif
		if (!skip_hypot) { ccm::fuzz::fuzz_binary_vs_std(in.x, in.y, ccm::fuzz::calls::hypot<T>, [](T a, T b) { return std::hypot(a, b); }); }

		ccm::fuzz::fuzz_binary_vs_std(in.x, in.y, ccm::fuzz::calls::pow<T>, [](T a, T b) { return std::pow(a, b); });
	}

	// Dedicated long double lane for pow only: powl has a native extended kernel, while the
	// other long double power overloads may delegate to double by policy and would diverge
	// from the native libm by more than the harness ulp bound.
	void power_long_double(uint8_t const * data, size_t size)
	{
		ccm::fuzz::Inputs<long double> in;
		if (!in.load_xy(data, size)) { return; }

		ccm::fuzz::fuzz_binary_vs_std(in.x, in.y, ccm::fuzz::calls::pow<long double>, [](long double a, long double b) { return std::pow(a, b); });
	}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(uint8_t const * data, size_t size)
{
	power<float>(data, size);
	power<double>(data, size);
	power_long_double(data, size);
	return 0;
}
