/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <gtest/gtest.h>

#include <ccmath/detail/basic/fma.hpp>
#include <cmath>


TEST(CcmathBasicTests, Fma)
{
    EXPECT_EQ(ccm::fma(1.0, 2.0, 3.0), std::fma(1.0, 2.0, 3.0));
}
