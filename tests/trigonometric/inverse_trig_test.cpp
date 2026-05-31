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
#include "utils/ulp_suite.hpp"

TEST(CcmathTrigonometricTests, InverseTrigConstexprSmoke)
{
	static_assert(ccm::acos(1.0) == 0.0);
	static_assert(ccm::asin(0.0) == 0.0);
	static_assert(ccm::atan(0.0) == 0.0);
	static_assert(ccm::atan2(0.0, 1.0) == 0.0);
}

TEST(CcmathTrigonometricTests, InverseTrigTypedAliases)
{
	EXPECT_FLOAT_EQ(ccm::acosf(0.5F), ccm::acos(0.5F));
	EXPECT_FLOAT_EQ(ccm::asinf(0.5F), ccm::asin(0.5F));
	EXPECT_FLOAT_EQ(ccm::atanf(1.0F), ccm::atan(1.0F));
	EXPECT_FLOAT_EQ(ccm::atan2f(1.0F, 1.0F), ccm::atan2(1.0F, 1.0F));
	EXPECT_DOUBLE_EQ(ccm::acosl(0.5L), ccm::acos(0.5L));
	EXPECT_DOUBLE_EQ(ccm::asinl(0.5L), ccm::asin(0.5L));
	EXPECT_DOUBLE_EQ(ccm::atanl(1.0L), ccm::atan(1.0L));
	EXPECT_DOUBLE_EQ(ccm::atan2l(1.0L, 1.0L), ccm::atan2(1.0L, 1.0L));
}

TEST(CcmathTrigonometricTests, InverseTrigIntegralPromotion)
{
	EXPECT_DOUBLE_EQ(ccm::acos(0), std::acos(0.0));
	EXPECT_DOUBLE_EQ(ccm::asin(0), std::asin(0.0));
	EXPECT_DOUBLE_EQ(ccm::atan(1), std::atan(1.0));
	EXPECT_DOUBLE_EQ(ccm::atan2(1.0, 1.0), std::atan2(1.0, 1.0));
}

TEST(CcmathTrigonometricTests, AsinAcosUnitDomainGridDouble)
{
	for (double input : ccm::test::samples::kInvTrigUnitDouble)
	{
		SCOPED_TRACE(input);
		ccm::test::ExpectUlpUnaryVsStd(input, ccm::asin<double>, static_cast<double (*)(double)>(std::asin));
		ccm::test::ExpectUlpUnaryVsStd(input, ccm::acos<double>, static_cast<double (*)(double)>(std::acos));
	}
}

TEST(CcmathTrigonometricTests, AsinAcosUnitDomainGridFloat)
{
	for (float input : ccm::test::samples::kInvTrigUnitFloat)
	{
		SCOPED_TRACE(input);
		ccm::test::ExpectUlpUnaryVsStd(input, ccm::asin<float>, static_cast<float (*)(float)>(std::asin));
		ccm::test::ExpectUlpUnaryVsStd(input, ccm::acos<float>, static_cast<float (*)(float)>(std::acos));
	}
}

TEST(CcmathTrigonometricTests, AtanGridDouble)
{
	for (double input : ccm::test::samples::kAtanDouble)
	{
		SCOPED_TRACE(input);
		ccm::test::ExpectUlpUnaryVsStd(input, ccm::atan<double>, static_cast<double (*)(double)>(std::atan));
	}
}

TEST(CcmathTrigonometricTests, Atan2QuadrantsDouble)
{
	for (std::size_t i = 0; i < ccm::test::samples::kAtan2YDouble.size(); ++i)
	{
		SCOPED_TRACE(ccm::test::samples::kAtan2YDouble[i]);
		SCOPED_TRACE(ccm::test::samples::kAtan2XDouble[i]);
		ccm::test::ExpectUlpBinaryVsStd(ccm::test::samples::kAtan2YDouble[i], ccm::test::samples::kAtan2XDouble[i], ccm::atan2<double>,
										static_cast<double (*)(double, double)>(std::atan2));
	}
}

TEST(CcmathTrigonometricTests, InverseTrigDomainEdgeCases)
{
	const double nan = std::numeric_limits<double>::quiet_NaN();
	const double inf = std::numeric_limits<double>::infinity();

	ccm::test::ExpectDomainEdgeMatchesStd(nan, ccm::asin<double>, static_cast<double (*)(double)>(std::asin));
	ccm::test::ExpectDomainEdgeMatchesStd(inf, ccm::asin<double>, static_cast<double (*)(double)>(std::asin));
	ccm::test::ExpectDomainEdgeMatchesStd(nan, ccm::acos<double>, static_cast<double (*)(double)>(std::acos));
	ccm::test::ExpectDomainEdgeMatchesStd(inf, ccm::acos<double>, static_cast<double (*)(double)>(std::acos));
	ccm::test::ExpectDomainEdgeMatchesStd(nan, ccm::atan<double>, static_cast<double (*)(double)>(std::atan));
	ccm::test::ExpectDomainEdgeMatchesStd(inf, ccm::atan<double>, static_cast<double (*)(double)>(std::atan));
	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(nan, 1.0), std::atan2(nan, 1.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(1.0, nan), std::atan2(1.0, nan));

	ccm::test::ExpectDomainEdgeMatchesStd(1.5, ccm::asin<double>, static_cast<double (*)(double)>(std::asin));
	ccm::test::ExpectDomainEdgeMatchesStd(-1.5, ccm::asin<double>, static_cast<double (*)(double)>(std::asin));
	ccm::test::ExpectDomainEdgeMatchesStd(1.5, ccm::acos<double>, static_cast<double (*)(double)>(std::acos));
	ccm::test::ExpectDomainEdgeMatchesStd(-1.5, ccm::acos<double>, static_cast<double (*)(double)>(std::acos));
}

TEST(CcmathTrigonometricTests, Atan2SignedZeroCases)
{
	const double pos_zero = 0.0;
	const double neg_zero = -0.0;

	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(pos_zero, pos_zero), std::atan2(pos_zero, pos_zero));
	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(neg_zero, pos_zero), std::atan2(neg_zero, pos_zero));
	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(pos_zero, neg_zero), std::atan2(pos_zero, neg_zero));
	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(neg_zero, neg_zero), std::atan2(neg_zero, neg_zero));

	const float pos_zero_f = 0.0F;
	const float neg_zero_f = -0.0F;
	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(pos_zero_f, neg_zero_f), std::atan2(pos_zero_f, neg_zero_f));
	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(neg_zero_f, neg_zero_f), std::atan2(neg_zero_f, neg_zero_f));

	const double inf = std::numeric_limits<double>::infinity();
	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(pos_zero, inf), std::atan2(pos_zero, inf));
	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(neg_zero, inf), std::atan2(neg_zero, inf));
	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(pos_zero, -inf), std::atan2(pos_zero, -inf));
	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(neg_zero, -inf), std::atan2(neg_zero, -inf));
}

TEST(CcmathTrigonometricTests, Atan2InfinityCases)
{
	const double inf = std::numeric_limits<double>::infinity();

	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(1.0, inf), std::atan2(1.0, inf));
	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(-1.0, inf), std::atan2(-1.0, inf));
	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(1.0, -inf), std::atan2(1.0, -inf));
	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(-1.0, -inf), std::atan2(-1.0, -inf));
	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(inf, 1.0), std::atan2(inf, 1.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(-inf, 1.0), std::atan2(-inf, 1.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(inf, -1.0), std::atan2(inf, -1.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(-inf, -1.0), std::atan2(-inf, -1.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(inf, inf), std::atan2(inf, inf));
	ccm::test::ExpectSameFloatingAsStd(ccm::atan2(-inf, inf), std::atan2(-inf, inf));
}

TEST(CcmathTrigonometricTests, InverseTrigBoundaryValues)
{
	ccm::test::ExpectSameFloatingAsStd(ccm::asin(1.0), std::asin(1.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::asin(-1.0), std::asin(-1.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::acos(1.0), std::acos(1.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::acos(-1.0), std::acos(-1.0));
}
