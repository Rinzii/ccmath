/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

//#include <cmath>
#include <iostream>

#include "ccmath/ccmath.hpp"

#include <cmath>


int main()
{


// EXPECT_EQ(ccm::signbit(-0.0), std::signbit(-0.0));

    constexpr double a = -0.0;
	constexpr double val = ccm::signbit(a);
	static_assert(val == true, "ccm::signbit(-0.0) != true");

	std::remquo(1.0, 2.0, nullptr);
	__builtin_remquo(1.0, 2.0, nullptr);
	// 	EXPECT_EQ(ccm::trunc(-std::numeric_limits<double>::quiet_NaN()), std::trunc(-std::numeric_limits<double>::quiet_NaN()));

	constexpr double b = -std::numeric_limits<double>::quiet_NaN();
	constexpr double val2 = ccm::trunc(b);
	static_assert(ccm::isnan(val2), "ccm::trunc(-std::numeric_limits<double>::quiet_NaN()) != std::trunc(-std::numeric_limits<double>::quiet_NaN())");

	// Test constexpr remquo
	constexpr double x = 1.0;
	constexpr double y = 2.0;
	static constexpr int * quo = nullptr;
	constexpr double result = ccm::remquo(x, y, quo);

    // Test trunc
    double b1 = std::trunc(-std::numeric_limits<double>::quiet_NaN());


	double b2 = ccm::trunc(-std::numeric_limits<double>::quiet_NaN());

    return 0;
}
