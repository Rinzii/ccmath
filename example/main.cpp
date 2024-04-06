/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include "ccmath/ccmath.hpp"
#include "cmath"
#include "iostream"

int main()
{
	auto test = ccm::log(1.0);

	//std::cout << test;

	// TODO: Implement actual examples showcasing the library

	unsigned int two = 2;

	unsigned int num = 5;
	unsigned int exp = 4;

	auto ccm_result = num * ccm::pow(two, exp);
	auto std_result = std::ldexp(num, static_cast<int>(exp));

	std::cout << "ccm::ldexp: " << ccm_result << std::endl;
	std::cout << "std::ldexp: " << std_result << std::endl;

    return 0;
}
