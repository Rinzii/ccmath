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
#include "ccmath/detail/nearest/trunc.hpp"
#include "ccmath/detail/basic/remainder.hpp"


// Clean up the global namespace
#include "ccmath/internal/setup/builtin_support_def.hpp"

#include <cmath>



int main()
{


// EXPECT_EQ(ccm::signbit(-0.0), std::signbit(-0.0));

    constexpr double a = -0.0;
	constexpr double val = ccm::signbit(a);
	static_assert(val == true, "ccm::signbit(-0.0) != true");

	ccm::fpclassify(a);

    std::cout << "ccm::signbit(-0.0) = " << a << std::endl;

	auto b = ccm::trunc(1.5);

	std::cout << "ccm::trunc(1.5) = " << b << std::endl;

	auto c = ccm::remainder(10.0, 3.0);

	std::cout << "ccm::remainder(10.0, 3.0) = " << c << std::endl;

	auto d = std::remainder(10.0, 3.0);

	std::cout << "std::remainder(10.0, 3.0) = " << d << std::endl;


    return 0;
}
