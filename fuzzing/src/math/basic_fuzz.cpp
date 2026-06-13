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
	void basic(uint8_t const * data, size_t size)
	{
		ccm::fuzz::Inputs<T> in;
		if (!in.load_xyz(data, size)) { return; }

		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::fabs<T>, [](T v) { return std::fabs(v); });
		ccm::fuzz::fuzz_binary_vs_std(in.x, in.y, ccm::fdim<T>, [](T a, T b) { return std::fdim(a, b); });
		ccm::fuzz::check_same_floating(ccm::fma(in.x, in.y, in.z), std::fma(in.x, in.y, in.z));
		ccm::fuzz::fuzz_binary_vs_std(in.x, in.y, ccm::fmod<T>, [](T a, T b) { return std::fmod(a, b); });
		ccm::fuzz::fuzz_binary_vs_std(in.x, in.y, ccm::max<T>, [](T a, T b) { return std::fmax(a, b); });
		ccm::fuzz::fuzz_binary_vs_std(in.x, in.y, ccm::min<T>, [](T a, T b) { return std::fmin(a, b); });
		ccm::fuzz::fuzz_binary_vs_std(in.x, in.y, ccm::remainder<T>, [](T a, T b) { return std::remainder(a, b); });
		ccm::fuzz::check_remquo_vs_std(in.x, in.y);
	}
} // namespace

CCMATH_FUZZ_DRIVER(basic)
