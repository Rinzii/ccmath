/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "io.hpp"

#include <ccmath/ccmath.hpp>

#include <cmath>

namespace
{
	constexpr double kInput = -2.375;
	static_assert(ccm::abs(kInput) == 2.375);
} // namespace

int main()
{
	ccm::examples::banner("constexpr abs");

	constexpr auto compiled = ccm::abs(kInput);
	const auto runtime		= ccm::abs(kInput);

	ccm::examples::print_row("input", kInput);
	ccm::examples::print_row("constexpr result", compiled);
	ccm::examples::print_row("runtime result", runtime);
	ccm::examples::print_row("std::abs reference", std::abs(kInput));

	return compiled == runtime && compiled == std::abs(kInput) ? 0 : 1;
}
