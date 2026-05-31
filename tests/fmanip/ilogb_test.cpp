/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <gtest/gtest.h>

#include <cmath>
#include <climits>
#include <cstdlib>
#include <limits>

#include "ccmath/ccmath.hpp"
#include "utils/math_samples.hpp"
#include "utils/std_compare.hpp"

TEST(CcmathFmanipTests, ILogbMatchesStdDouble)
{
	for (double input : ccm::test::samples::kIlogbProbeDouble)
	{
		SCOPED_TRACE(input);
		EXPECT_EQ(ccm::ilogb(input), std::ilogb(input));
	}
}

TEST(CcmathFmanipTests, ILogbMatchesStdFloat)
{
	for (float input : ccm::test::samples::kIlogbProbeFloat)
	{
		SCOPED_TRACE(input);
		EXPECT_EQ(ccm::ilogb(input), std::ilogb(input));
	}
}

TEST(CcmathFmanipTests, ILogbSpecialValues)
{
	EXPECT_EQ(ccm::ilogb(std::numeric_limits<double>::quiet_NaN()), std::ilogb(std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(ccm::ilogb(std::numeric_limits<double>::infinity()), std::ilogb(std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::ilogb(-std::numeric_limits<double>::infinity()), std::ilogb(-std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::ilogb(0.0), std::ilogb(0.0));
	EXPECT_EQ(ccm::ilogb(-0.0), std::ilogb(-0.0));
}

TEST(CcmathFmanipTests, ILogbTypedAliases)
{
	EXPECT_EQ(ccm::ilogbf(8.0F), ccm::ilogb(8.0F));
	EXPECT_EQ(ccm::ilogbl(8.0L), ccm::ilogb(8.0L));
}

TEST(CcmathFmanipTests, ILogbSubnormalExponent)
{
	const double denorm = std::numeric_limits<double>::denorm_min();
	EXPECT_EQ(ccm::ilogb(denorm), std::ilogb(denorm));
	EXPECT_EQ(ccm::ilogb(-denorm), std::ilogb(-denorm));
}

TEST(CcmathFmanipTests, ILogbCompileTime)
{
	static_assert(ccm::ilogb(8.0) == 3);
	static_assert(ccm::ilogb(0.125) == -3);
}
