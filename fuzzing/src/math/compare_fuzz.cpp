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
	template <typename T> void compare(uint8_t const * data, size_t size)
	{
		ccm::fuzz::Inputs<T> in;
		if (!in.load_xy(data, size))
		{
			return;
		}

		FUZZ_CHECK(ccm::fpclassify(in.x) == std::fpclassify(in.x));
		FUZZ_CHECK(static_cast<bool>(ccm::isfinite(in.x)) == static_cast<bool>(std::isfinite(in.x)));
		FUZZ_CHECK(static_cast<bool>(ccm::isinf(in.x)) == static_cast<bool>(std::isinf(in.x)));
		FUZZ_CHECK(static_cast<bool>(ccm::isnan(in.x)) == static_cast<bool>(std::isnan(in.x)));
		FUZZ_CHECK(static_cast<bool>(ccm::isnormal(in.x)) == static_cast<bool>(std::isnormal(in.x)));
		FUZZ_CHECK(static_cast<bool>(ccm::signbit(in.x)) == static_cast<bool>(std::signbit(in.x)));

		ccm::fuzz::fuzz_binary_exact_int(in.x, in.y, ccm::isgreater<T>, [](T a, T b) { return std::isgreater(a, b); });
		ccm::fuzz::fuzz_binary_exact_int(in.x, in.y, ccm::isgreaterequal<T>, [](T a, T b) { return std::isgreaterequal(a, b); });
		ccm::fuzz::fuzz_binary_exact_int(in.x, in.y, ccm::isless<T>, [](T a, T b) { return std::isless(a, b); });
		ccm::fuzz::fuzz_binary_exact_int(in.x, in.y, ccm::islessequal<T>, [](T a, T b) { return std::islessequal(a, b); });
		ccm::fuzz::fuzz_binary_exact_int(in.x, in.y, ccm::islessgreater<T>, [](T a, T b) { return std::islessgreater(a, b); });
		ccm::fuzz::fuzz_binary_exact_int(in.x, in.y, ccm::isunordered<T>, [](T a, T b) { return std::isunordered(a, b); });
	}
} // namespace

CCMATH_FUZZ_DRIVER_ALL_TYPES(compare)
