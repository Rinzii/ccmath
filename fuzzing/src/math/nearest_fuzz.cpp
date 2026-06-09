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
	void nearest(uint8_t const * data, size_t size)
	{
		ccm::fuzz::Inputs<T> in;
		if (!in.load_x(data, size)) { return; }

		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::floor<T>, [](T v) { return std::floor(v); });
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::trunc<T>, [](T v) { return std::trunc(v); });
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::nearbyint<T>, [](T v) { return std::nearbyint(v); });
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::ceil<T>, [](T v) { return std::ceil(v); });
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::round<T>, [](T v) { return std::round(v); });
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::rint<T>, [](T v) { return std::rint(v); });
	}
} // namespace

CCMATH_FUZZ_DRIVER(nearest)
