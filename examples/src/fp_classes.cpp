/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "io.hpp"

#include <ccmath/ccmath.hpp>

#include <limits>
#include <string_view>

namespace
{
	std::string_view classify(double x)
	{
		switch (ccm::fpclassify(x))
		{
		case FP_INFINITE: return "infinite";
		case FP_NAN: return "nan";
		case FP_NORMAL: return "normal";
		case FP_SUBNORMAL: return "subnormal";
		case FP_ZERO: return "zero";
		default: return "unknown";
		}
	}

	double const kValues[] = {
		1.0,
		0.0,
		-0.0,
		std::numeric_limits<double>::denorm_min(),
		std::numeric_limits<double>::infinity(),
		-std::numeric_limits<double>::infinity(),
		std::numeric_limits<double>::quiet_NaN(),
	};
} // namespace

int main()
{
	ccm::examples::banner("floating-point classes");

	ccm::examples::table_header("value", "class", "signbit");

	for (double v : kValues)
	{
		std::cout << std::left << std::setw(14) << v << std::setw(18) << classify(v) << std::setw(18) << ccm::signbit(v) << '\n';
	}

	ccm::examples::section("ordering predicates");
	ccm::examples::print_row("isfinite(1)", ccm::isfinite(1.0));
	ccm::examples::print_row("isnan(NaN)", ccm::isnan(std::numeric_limits<double>::quiet_NaN()));

	return 0;
}
