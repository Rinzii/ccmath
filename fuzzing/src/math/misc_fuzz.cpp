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
	template <typename T> T reference_lerp(T a, T b, T t)
	{
		if (std::isnan(a) || std::isnan(b) || std::isnan(t))
		{
			return a + b + t;
		}

		if ((a <= T{} && b >= T{}) || (a >= T{} && b <= T{}))
		{
			return t * b + (static_cast<T>(1) - t) * a;
		}
		if (t == static_cast<T>(1))
		{
			return b;
		}

		T const x = a + t * (b - a);
		if ((t > static_cast<T>(1)) == (b > a))
		{
			return b < x ? x : b;
		}
		return x < b ? x : b;
	}

	template <typename T> void misc(uint8_t const * data, size_t size)
	{
		ccm::fuzz::Inputs<T> in;
		if (!in.load_xyz(data, size))
		{
			return;
		}

		ccm::fuzz::check_same_floating(ccm::lerp(in.x, in.y, in.z), reference_lerp(in.x, in.y, in.z));

		if (in.x < T{} && in.x == std::trunc(in.x))
		{
			return;
		}
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::gamma<T>, [](T v) { return std::tgamma(v); });
		ccm::fuzz::fuzz_unary_vs_std(in.x, ccm::lgamma<T>, [](T v) { return std::lgamma(v); });
	}
} // namespace

CCMATH_FUZZ_DRIVER(misc)
