/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

//#include <cmath>
#include <ccmath/detail/compare/signbit.hpp>
#include <iostream>

#include "ccmath/detail/compare/fpclassify.hpp"

// Clean up the global namespace
#include "ccmath/internal/setup/builtin_support_def.hpp"



int main()
{


// EXPECT_EQ(ccm::signbit(-0.0), std::signbit(-0.0));

    constexpr double a = -0.0;
	constexpr double val = ccm::signbit(a);
	static_assert(val == true, "ccm::signbit(-0.0) != true");

	ccm::fpclassify(a);

    std::cout << "ccm::signbit(-0.0) = " << a << std::endl;


    return 0;
}
