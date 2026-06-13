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

namespace
{
	template <typename T>
	void trig(uint8_t const * data, size_t size)
	{
		ccm::fuzz::Inputs<T> in;
		if (!in.load_xy(data, size)) { return; }

#ifndef CCMATH_FUZZ_GENERIC
		// Public-dispatch build: sin / cos / tan are differential-tested against libm.
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::fuzz::calls::sin<T>, [](T v) { return std::sin(v); });
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::fuzz::calls::cos<T>, [](T v) { return std::cos(v); });
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::fuzz::calls::tan<T>, [](T v) { return std::tan(v); });
#endif
		// TODO(IanP): arm sin / cos / tan in the generic build once the trig kernel overhaul
		// chip lands. The double kernel is only float-grade (sin(1.5) is off by ~1.5e7 ulp),
		// large-argument reduction collapses to multiples of pi/8 in both precisions, and
		// sin / tan return the wrong sign of zero.

		// Inverse trig has no generic kernel yet, so it always exercises the public dispatch.
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::fuzz::calls::asin<T>, [](T v) { return std::asin(v); });
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::fuzz::calls::acos<T>, [](T v) { return std::acos(v); });
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::fuzz::calls::atan<T>, [](T v) { return std::atan(v); });
		ccm::fuzz::fuzz_binary_vs_std(in.x, in.y, ccm::fuzz::calls::atan2<T>, [](T a, T b) { return std::atan2(a, b); });
	}
} // namespace

CCMATH_FUZZ_DRIVER(trig)
