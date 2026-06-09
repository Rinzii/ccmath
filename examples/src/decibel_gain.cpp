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
	double power_ratio_to_db(double ratio)
	{ return 10.0 * ccm::log10(ratio); }

	double amplitude_ratio_to_db(double ratio)
	{ return 20.0 * ccm::log10(ratio); }
} // namespace

int main()
{
	ccm::examples::banner("decibel conversion");

	double const ratios[] = {1.0, 2.0, 10.0, 100.0};

	ccm::examples::table_header("power ratio", "dB (10 log10)");

	for (double ratio : ratios) { std::cout << std::left << std::setw(14) << ratio << std::setw(18) << power_ratio_to_db(ratio) << '\n'; }

	ccm::examples::section("amplitude form (20 log10)");
	ccm::examples::print_row("2x amplitude", amplitude_ratio_to_db(2.0));

	return 0;
}
