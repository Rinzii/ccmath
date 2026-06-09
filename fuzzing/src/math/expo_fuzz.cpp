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
	void expo(uint8_t const * data, size_t size)
	{
		ccm::fuzz::Inputs<T> in;
		if (!in.load_x(data, size)) { return; }

		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::exp<T>, [](T v) { return std::exp(v); });
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::exp2<T>, [](T v) { return std::exp2(v); });
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::expm1<T>, [](T v) { return std::expm1(v); });
		ccm::fuzz::fuzz_unary_log_family(in.x, ccm::log<T>, [](T v) { return std::log(v); });
		ccm::fuzz::fuzz_unary_log1p_family(in.x, ccm::log1p<T>, [](T v) { return std::log1p(v); });
		ccm::fuzz::fuzz_unary_log_family(in.x, ccm::log2<T>, [](T v) { return std::log2(v); });
		ccm::fuzz::fuzz_unary_log_family(in.x, ccm::log10<T>, [](T v) { return std::log10(v); });
	}
} // namespace

CCMATH_FUZZ_DRIVER(expo)
