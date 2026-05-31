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
#include <limits>

#include "ccmath/ccmath.hpp"
#include "utils/math_samples.hpp"
#include "utils/std_compare.hpp"

TEST(CcmathFmanipTests, LogbMatchesStdDouble)
{
	for (double input : ccm::test::samples::kLogbProbeDouble)
	{
		SCOPED_TRACE(input);
		ccm::test::ExpectSameAsStd(ccm::logb(input), std::logb(input));
	}
}

TEST(CcmathFmanipTests, LogbSpecialValues)
{
	ccm::test::ExpectSameAsStd(ccm::logb(std::numeric_limits<double>::quiet_NaN()), std::logb(std::numeric_limits<double>::quiet_NaN()));
	ccm::test::ExpectSameAsStd(ccm::logb(std::numeric_limits<double>::infinity()), std::logb(std::numeric_limits<double>::infinity()));
	ccm::test::ExpectSameAsStd(ccm::logb(-std::numeric_limits<double>::infinity()), std::logb(-std::numeric_limits<double>::infinity()));
	ccm::test::ExpectSameAsStd(ccm::logb(0.0), std::logb(0.0));
	ccm::test::ExpectSameAsStd(ccm::logb(-0.0), std::logb(-0.0));
}

TEST(CcmathFmanipTests, LogbTypedAliases)
{
	ccm::test::ExpectSameAsStd(ccm::logbf(8.0F), std::logbf(8.0F));
	ccm::test::ExpectSameAsStd(ccm::logbl(8.0L), std::logbl(8.0L));
}

TEST(CcmathFmanipTests, LogbSubnormal)
{
	const double denorm = std::numeric_limits<double>::denorm_min();
	ccm::test::ExpectSameAsStd(ccm::logb(denorm), std::logb(denorm));
	ccm::test::ExpectSameAsStd(ccm::logb(-denorm), std::logb(-denorm));
}

TEST(CcmathFmanipTests, LogbCompileTime)
{
	static_assert(ccm::logb(8.0) == 3.0);
	static_assert(ccm::logb(0.125) == -3.0);
}
