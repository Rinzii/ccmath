/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <gtest/gtest.h>

#include <ccmath/detail/basic/fmod.hpp>
#include <cmath>
#include <limits>


TEST(CcmathBasicTests, Fmod)
{


    // Test fmod with floating point numbers
    EXPECT_FLOAT_EQ(ccm::fmod(10.0f, 3.0f), std::fmod(10.0f, 3.0f)); // NOLINT
    EXPECT_FLOAT_EQ(ccm::fmod(10.0f, -3.0f), std::fmod(10.0f, -3.0f));
    EXPECT_FLOAT_EQ(ccm::fmod(-10.0f, 3.0f), std::fmod(-10.0f, 3.0f));
    EXPECT_FLOAT_EQ(ccm::fmod(-10.0f, -3.0f), std::fmod(-10.0f, -3.0f));
    //EXPECT_FLOAT_EQ(ccm::fmod(10.0f, 0.0f), std::fmod(10.0f, 0.0f)); // Not testable due to implementation defined behavior
    EXPECT_FLOAT_EQ(ccm::fmod(0.0f, 3.0f), std::fmod(0.0f, 3.0f));
    EXPECT_FLOAT_EQ(ccm::fmod(0.0f, 0.0f), std::fmod(0.0f, 0.0f));

}
