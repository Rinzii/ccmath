/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "io.hpp"

#include <ccmath/ccmath.hpp>

namespace
{
	struct Pair
	{
		double a;
		double b;
	};

	constexpr Pair kCases[] = {
		{ 3.0, -1.0 },
		{ -4.0, -4.0 },
		{ 0.0, -0.0 },
	};
} // namespace

int main()
{
	ccm::examples::banner("signed magnitude");

	ccm::examples::table_header("a", "b", "fdim(a,b)");

	for (auto const & c : kCases)
	{
		double const positive_diff = ccm::fdim(c.a, c.b);
		double const magnitude	   = ccm::fabs(c.a);
		double const sign		   = ccm::copysign(1.0, c.a);

		std::cout << std::left << std::setw(14) << c.a << std::setw(18) << c.b << std::setw(18) << positive_diff << '\n';
		ccm::examples::print_row("  fabs(a)", magnitude);
		ccm::examples::print_row("  copysign(1,a)", sign);
	}

	return 0;
}
