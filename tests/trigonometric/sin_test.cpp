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
#include "utils/conformance_suite.hpp"
#include "utils/ulp_suite.hpp"

TEST(CcmathTrigonometricTests, ConstexprSmoke)
{
	static_assert(ccm::sin(0.0f) == 0.0f, "ccm::sin(0) must be 0 at compile time");
	static_assert(ccm::cos(0.0f) == 1.0f, "ccm::cos(0) must be 1 at compile time");
	static_assert(ccm::tan(0.0f) == 0.0f, "ccm::tan(0) must be 0 at compile time");
}

TEST(CcmathTrigonometricTests, TypedAliases)
{
	EXPECT_FLOAT_EQ(ccm::sinf(1.0f), ccm::sin(1.0f));
	EXPECT_FLOAT_EQ(ccm::cosf(1.0f), ccm::cos(1.0f));
	EXPECT_FLOAT_EQ(ccm::tanf(1.0f), ccm::tan(1.0f));
	EXPECT_DOUBLE_EQ(ccm::sinl(1.0L), ccm::sin(1.0L));
	EXPECT_DOUBLE_EQ(ccm::cosl(1.0L), ccm::cos(1.0L));
	EXPECT_DOUBLE_EQ(ccm::tanl(1.0L), ccm::tan(1.0L));
}

TEST(CcmathTrigonometricTests, IntegralPromotion)
{
	EXPECT_DOUBLE_EQ(ccm::sin(2), std::sin(2.0));
	EXPECT_DOUBLE_EQ(ccm::cos(2), std::cos(2.0));
	EXPECT_DOUBLE_EQ(ccm::tan(2), std::tan(2.0));
}

TEST(CcmathTrigonometricTests, DomainEdgeCasesMatchLibm)
{
	const float nan_f = std::numeric_limits<float>::quiet_NaN();
	const float inf_f = std::numeric_limits<float>::infinity();
	const double nan_d = std::numeric_limits<double>::quiet_NaN();
	const double inf_d = std::numeric_limits<double>::infinity();

	ccm::test::ExpectDomainEdgeMatchesStd(nan_f, ccm::sin<float>, static_cast<float (*)(float)>(std::sin));
	ccm::test::ExpectDomainEdgeMatchesStd(inf_f, ccm::sin<float>, static_cast<float (*)(float)>(std::sin));
	ccm::test::ExpectDomainEdgeMatchesStd(nan_f, ccm::cos<float>, static_cast<float (*)(float)>(std::cos));
	ccm::test::ExpectDomainEdgeMatchesStd(inf_f, ccm::cos<float>, static_cast<float (*)(float)>(std::cos));
	ccm::test::ExpectDomainEdgeMatchesStd(nan_f, ccm::tan<float>, static_cast<float (*)(float)>(std::tan));
	ccm::test::ExpectDomainEdgeMatchesStd(inf_f, ccm::tan<float>, static_cast<float (*)(float)>(std::tan));

	ccm::test::ExpectDomainEdgeMatchesStd(nan_d, ccm::sin<double>, static_cast<double (*)(double)>(std::sin));
	ccm::test::ExpectDomainEdgeMatchesStd(inf_d, ccm::sin<double>, static_cast<double (*)(double)>(std::sin));
	ccm::test::ExpectDomainEdgeMatchesStd(nan_d, ccm::cos<double>, static_cast<double (*)(double)>(std::cos));
	ccm::test::ExpectDomainEdgeMatchesStd(inf_d, ccm::cos<double>, static_cast<double (*)(double)>(std::cos));
	ccm::test::ExpectDomainEdgeMatchesStd(nan_d, ccm::tan<double>, static_cast<double (*)(double)>(std::tan));
	ccm::test::ExpectDomainEdgeMatchesStd(inf_d, ccm::tan<double>, static_cast<double (*)(double)>(std::tan));
}

TEST(CcmathTrigonometricTests, LargeMagnitudeReductionMatchesStd)
{
	const double huge = 0x1.0p500;
	ccm::test::ExpectSameFloatingAsStd(ccm::sin(huge), std::sin(huge));
	ccm::test::ExpectSameFloatingAsStd(ccm::cos(huge), std::cos(huge));
	ccm::test::ExpectSameFloatingAsStd(ccm::sin(-huge), std::sin(-huge));
}
