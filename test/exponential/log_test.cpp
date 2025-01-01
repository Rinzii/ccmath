/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "ccmath/ccmath.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <limits>

TEST(CcmathExponentialTests, Log)
{
	// Verify function is static assert-able
	// 1.3862943611198906 was generated with std::log(4.0)
	static_assert(ccm::log(4.0) == 1.3862943611198906, "log has failed testing that it is static_assert-able!");

	EXPECT_EQ(ccm::log(1.0), std::log(1.0));
	EXPECT_EQ(ccm::log(2.0), std::log(2.0));
	EXPECT_EQ(ccm::log(4.0), std::log(4.0));
	EXPECT_EQ(ccm::log(8.0), std::log(8.0));
	EXPECT_EQ(ccm::log(16.0), std::log(16.0));
	EXPECT_EQ(ccm::log(32.0), std::log(32.0));
	EXPECT_EQ(ccm::log(64.0), std::log(64.0));
	EXPECT_EQ(ccm::log(128.0), std::log(128.0));
	EXPECT_EQ(ccm::log(256.0), std::log(256.0));
	EXPECT_EQ(ccm::log(512.0), std::log(512.0));
	EXPECT_EQ(ccm::log(1024.0), std::log(1024.0));
	EXPECT_EQ(ccm::log(2048.0), std::log(2048.0));
	EXPECT_EQ(ccm::log(4096.0), std::log(4096.0));
	EXPECT_EQ(ccm::log(8192.0), std::log(8192.0));
	EXPECT_EQ(ccm::log(16384.0), std::log(16384.0));
	EXPECT_EQ(ccm::log(32768.0), std::log(32768.0));
	EXPECT_EQ(ccm::log(65536.0), std::log(65536.0));
	EXPECT_EQ(ccm::log(131072.0), std::log(131072.0));
	EXPECT_EQ(ccm::log(262144.0), std::log(262144.0));
	EXPECT_EQ(ccm::log(524288.0), std::log(524288.0));
	EXPECT_EQ(ccm::log(1048576.0), std::log(1048576.0));

	// Check for edge cases
	bool ccmCheckForNan = std::isnan(ccm::log(std::numeric_limits<double>::quiet_NaN()));
	bool stdCheckForNan = std::isnan(std::log(std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(ccmCheckForNan, stdCheckForNan);
	EXPECT_EQ(ccm::log(std::numeric_limits<double>::infinity()), std::log(std::numeric_limits<double>::infinity()));
	bool ccmCheckForNegativeNan = std::isnan(ccm::log(-1.0));
	bool stdCheckForNegativeNan = std::isnan(std::log(-1.0));
	EXPECT_EQ(ccmCheckForNegativeNan, stdCheckForNegativeNan);
	EXPECT_EQ(ccm::log(0.0), std::log(0.0));
	EXPECT_EQ(ccm::log(-0.0), std::log(-0.0));
}
