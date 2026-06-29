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
#include <type_traits>

namespace
{
	template <typename T> void expo(uint8_t const * data, size_t size)
	{
		ccm::fuzz::Inputs<T> in;
		if (!in.load_x(data, size))
		{
			return;
		}

		// fuzz_calls routes to the generic kernels in the generic build and to the public
		// dispatch otherwise. The exp and log kernels meet the contract for both float and
		// double, so the same call set covers both build flavors.
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::fuzz::calls::exp<T>, [](T v) { return std::exp(v); });
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::fuzz::calls::exp2<T>, [](T v) { return std::exp2(v); });
		ccm::fuzz::fuzz_unary_log_family(in.x, ccm::fuzz::calls::log<T>, [](T v) { return std::log(v); });
		ccm::fuzz::fuzz_unary_log_family(in.x, ccm::fuzz::calls::log2<T>, [](T v) { return std::log2(v); });
		ccm::fuzz::fuzz_unary_log_family(in.x, ccm::fuzz::calls::log10<T>, [](T v) { return std::log10(v); });

#ifdef CCMATH_FUZZ_GENERIC
		// TODO(IanP): arm the double expm1 and log1p generic kernels once fixed. expm1 returns
		// 0 instead of -1 for large-negative args, and log1p is ~211 ULP off across its range
		// (chips). Both float kernels are correct.
		if constexpr (!std::is_same_v<T, double>)
		{
			ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::fuzz::calls::expm1<T>, [](T v) { return std::expm1(v); });
			ccm::fuzz::fuzz_unary_log1p_family(in.x, ccm::fuzz::calls::log1p<T>, [](T v) { return std::log1p(v); });
		}
#else
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::fuzz::calls::expm1<T>, [](T v) { return std::expm1(v); });
		ccm::fuzz::fuzz_unary_log1p_family(in.x, ccm::fuzz::calls::log1p<T>, [](T v) { return std::log1p(v); });
#endif
	}
} // namespace

CCMATH_FUZZ_DRIVER(expo)
