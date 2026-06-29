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
	constexpr double kPi = 3.14159265358979323846;

	struct AngleCase
	{
		double degrees;
		double radians;
	};

	constexpr AngleCase kAngles[] = {
		{ 0.0, 0.0 }, { 30.0, kPi / 6.0 }, { 45.0, kPi / 4.0 }, { 60.0, kPi / 3.0 }, { 90.0, kPi / 2.0 },
	};
} // namespace

int main()
{
	ccm::examples::banner("unit circle samples");

	ccm::examples::table_header("deg", "sin", "cos");

	for (auto const & angle : kAngles)
	{
		double const s = ccm::sin(angle.radians);
		double const c = ccm::cos(angle.radians);
		std::cout << std::left << std::setw(14) << angle.degrees << std::setw(18) << s << std::setw(18) << c << '\n';
	}

	ccm::examples::section("identity check");
	double const x	 = kPi / 5.0;
	double const lhs = ccm::sin(x) * ccm::sin(x) + ccm::cos(x) * ccm::cos(x);
	ccm::examples::print_row("sin^2(x) + cos^2(x)", lhs);

	return 0;
}
