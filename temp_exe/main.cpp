/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <cmath>
#include <ccmath/detail/compare/signbit.hpp>
#include <iostream>

int main()
{


// EXPECT_EQ(ccm::signbit(-0.0), std::signbit(-0.0));

    auto a = ccm::signbit(-0.0);

    std::cout << "ccm::signbit(-0.0) = " << a << std::endl;
	std::cout << "std::signbit(0.0) = " << std::signbit(0.0) << std::endl;

    return 0;
}
