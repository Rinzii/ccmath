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
	template <typename T> void fmanip(uint8_t const * data, size_t size)
	{
		ccm::fuzz::Inputs<T> in;
		if (!in.load_xy_i(data, size))
		{
			return;
		}

		ccm::fuzz::fuzz_binary_vs_std(in.x, in.y, ccm::copysign<T>, [](T a, T b) { return std::copysign(a, b); });
		ccm::fuzz::fuzz_binary_vs_std(in.x, in.y, ccm::nextafter<T>, [](T a, T b) { return std::nextafter(a, b); });
		long double const toward = static_cast<long double>(in.y);
		ccm::fuzz::check_same_floating(ccm::nexttoward(in.x, toward), std::nexttoward(in.x, toward));
		ccm::fuzz::check_same_floating(ccm::scalbn(in.x, in.i), std::scalbn(in.x, in.i));
		ccm::fuzz::check_same_floating(ccm::ldexp(in.x, in.i), std::ldexp(in.x, in.i));

		ccm::fuzz::check_frexp_vs_std(in.x);
		ccm::fuzz::check_modf_vs_std(in.x);

		FUZZ_CHECK(ccm::ilogb(in.x) == std::ilogb(in.x));
		ccm::fuzz::check_same_floating(ccm::logb(in.x), std::logb(in.x));
	}
} // namespace

CCMATH_FUZZ_DRIVER_ALL_TYPES(fmanip)
