/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <gtest/gtest.h>

#include <ccmath/ccmath.hpp>
#include <cmath>
#include <limits>

TEST(CcmathBasicTests, FdimStaticAssert)
{
	static_assert(ccm::fdim(1.0, 1.0) == 0.0, "fdim has failed testing that it is static_assert-able!");
}

TEST(CcmathBasicTests, FdimDouble)
{
	EXPECT_EQ(ccm::fdim(1.0, 1.0), std::fdim(1.0, 1.0));
	EXPECT_EQ(ccm::fdim(2.0, 2.0), std::fdim(2.0, 2.0));
	EXPECT_EQ(ccm::fdim(4.0, 4.0), std::fdim(4.0, 4.0));
	EXPECT_EQ(ccm::fdim(8.0, 8.0), std::fdim(8.0, 8.0));
	EXPECT_EQ(ccm::fdim(16.0, 16.0), std::fdim(16.0, 16.0));
	EXPECT_EQ(ccm::fdim(32.0, 32.0), std::fdim(32.0, 32.0));

	EXPECT_EQ(ccm::fdim(1.0, 0.0), std::fdim(1.0, 0.0));
	EXPECT_EQ(ccm::fdim(0.0, 1.0), std::fdim(0.0, 1.0));
	EXPECT_EQ(ccm::fdim(0.0, 0.0), std::fdim(0.0, 0.0));
	EXPECT_EQ(ccm::fdim(-1.0, 1.0), std::fdim(-1.0, 1.0));
	EXPECT_EQ(ccm::fdim(1.0, -1.0), std::fdim(1.0, -1.0));
	EXPECT_EQ(ccm::fdim(-1.0, -1.0), std::fdim(-1.0, -1.0));
}

TEST(CcmathBasicTests, FdimFloat)
{
    EXPECT_EQ(ccm::fdim(1.0F, 1.0F), std::fdim(1.0F, 1.0F));
    EXPECT_EQ(ccm::fdim(2.0F, 2.0F), std::fdim(2.0F, 2.0F));
    EXPECT_EQ(ccm::fdim(4.0F, 4.0F), std::fdim(4.0F, 4.0F));
    EXPECT_EQ(ccm::fdim(8.0F, 8.0F), std::fdim(8.0F, 8.0F));
    EXPECT_EQ(ccm::fdim(16.0F, 16.0F), std::fdim(16.0F, 16.0F));
    EXPECT_EQ(ccm::fdim(32.0F, 32.0F), std::fdim(32.0F, 32.0F));

    EXPECT_EQ(ccm::fdim(1.0F, 0.0F), std::fdim(1.0F, 0.0F));
    EXPECT_EQ(ccm::fdim(0.0F, 1.0F), std::fdim(0.0F, 1.0F));
    EXPECT_EQ(ccm::fdim(0.0F, 0.0F), std::fdim(0.0F, 0.0F));
    EXPECT_EQ(ccm::fdim(-1.0F, 1.0F), std::fdim(-1.0F, 1.0F));
    EXPECT_EQ(ccm::fdim(1.0F, -1.0F), std::fdim(1.0F, -1.0F));
    EXPECT_EQ(ccm::fdim(-1.0F, -1.0F), std::fdim(-1.0F, -1.0F));
}

TEST(CcmathBasicTests, FdimLongDouble)
{
    EXPECT_EQ(ccm::fdim(1.0L, 1.0L), std::fdim(1.0L, 1.0L));
    EXPECT_EQ(ccm::fdim(2.0L, 2.0L), std::fdim(2.0L, 2.0L));
    EXPECT_EQ(ccm::fdim(4.0L, 4.0L), std::fdim(4.0L, 4.0L));
    EXPECT_EQ(ccm::fdim(8.0L, 8.0L), std::fdim(8.0L, 8.0L));
    EXPECT_EQ(ccm::fdim(16.0L, 16.0L), std::fdim(16.0L, 16.0L));
    EXPECT_EQ(ccm::fdim(32.0L, 32.0L), std::fdim(32.0L, 32.0L));

    EXPECT_EQ(ccm::fdim(1.0L, 0.0L), std::fdim(1.0L, 0.0L));
    EXPECT_EQ(ccm::fdim(0.0L, 1.0L), std::fdim(0.0L, 1.0L));
    EXPECT_EQ(ccm::fdim(0.0L, 0.0L), std::fdim(0.0L, 0.0L));
    EXPECT_EQ(ccm::fdim(-1.0L, 1.0L), std::fdim(-1.0L, 1.0L));
    EXPECT_EQ(ccm::fdim(1.0L, -1.0L), std::fdim(1.0L, -1.0L));
    EXPECT_EQ(ccm::fdim(-1.0L, -1.0L), std::fdim(-1.0L, -1.0L));
}

TEST(CcmathBasicTests, FdimEdgeCases)
{

}
