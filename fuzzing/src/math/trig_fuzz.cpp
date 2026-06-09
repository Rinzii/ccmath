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
	void trig(uint8_t const * data, size_t size)
	{
		ccm::fuzz::Inputs<T> in;
		if (!in.load_xy(data, size)) { return; }

		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::sin<T>, [](T v) { return std::sin(v); });
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::cos<T>, [](T v) { return std::cos(v); });
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::tan<T>, [](T v) { return std::tan(v); });
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::asin<T>, [](T v) { return std::asin(v); });
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::acos<T>, [](T v) { return std::acos(v); });
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::atan<T>, [](T v) { return std::atan(v); });
		ccm::fuzz::fuzz_binary_vs_std(in.x, in.y, ccm::atan2<T>, [](T a, T b) { return std::atan2(a, b); });
	}
} // namespace

CCMATH_FUZZ_DRIVER(trig)
