/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <ccmath/ccmath.hpp>

static_assert(ccm::fabs(-1.0) == 1.0);

int main()
{
	constexpr auto value = ccm::sin(0.5);
	return value > 0.0 ? 0 : 1;
}
