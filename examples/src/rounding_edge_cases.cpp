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
	struct Sample
	{
		double x;
	};

	constexpr Sample kSamples[] = { { 2.5 }, { 3.5 }, { -2.5 }, { 1e8 + 0.5 } };
} // namespace

int main()
{
	ccm::examples::banner("rounding neighbors");

	ccm::examples::table_header("x", "nearbyint", "round");

	for (auto const & s : kSamples)
	{
		std::cout << std::left << std::setw(14) << s.x << std::setw(18) << ccm::nearbyint(s.x) << std::setw(18) << ccm::round(s.x) << '\n';
	}

	ccm::examples::section("floor / ceil / trunc at pi");
	double const x = 3.141592653589793;
	ccm::examples::print_row("floor", ccm::floor(x));
	ccm::examples::print_row("ceil", ccm::ceil(x));
	ccm::examples::print_row("trunc", ccm::trunc(x));

	return 0;
}
