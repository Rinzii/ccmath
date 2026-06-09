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
#include "harness.hpp"

#include <ccmath/ccmath.hpp>

#include <cmath>

namespace
{
	template <typename T>
	void power(uint8_t const * data, size_t size)
	{
		ccm::fuzz::Inputs<T> in;
		if (!in.load_xy(data, size)) { return; }

		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::sqrt<T>, [](T v) { return std::sqrt(v); });
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::cbrt<T>, [](T v) { return std::cbrt(v); });
		ccm::fuzz::fuzz_binary_vs_std(in.x, in.y, ccm::pow<T>, [](T a, T b) { return std::pow(a, b); });
		ccm::fuzz::fuzz_binary_vs_std(in.x, in.y, ccm::hypot<T>, [](T a, T b) { return std::hypot(a, b); });
	}
} // namespace

CCMATH_FUZZ_DRIVER(power)
