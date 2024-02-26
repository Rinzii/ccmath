/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <cmath>
#include <ccmath/detail/basic/fmod.hpp>
#include <iostream>

#include <math.h>

int main()
{

	auto test = std::fmod(10.0f, 0.0f);
	auto test2 = ccm::fmod(10.0f, 0.0f);


	std::cout << "std::fmod(10.0f, 0.0f) = " << test << std::endl;
	std::cout << "ccm::fmod(10.0f, 0.0f) = " << test2 << std::endl;
    return 0;
}
