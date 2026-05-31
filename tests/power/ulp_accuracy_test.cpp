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

TEST(CcmathPowerUlpTests, SqrtDouble)
{
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kSqrtDouble, ccm::sqrt<double>, static_cast<double (*)(double)>(std::sqrt));
}

TEST(CcmathPowerUlpTests, SqrtFloat)
{
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kSqrtFloat, ccm::sqrt<float>, static_cast<float (*)(float)>(std::sqrt));
}

TEST(CcmathPowerUlpTests, PowDouble)
{
	for (double base : ccm::test::samples::kPowBases)
	{
		for (double exp : ccm::test::samples::kPowExponents)
		{
			SCOPED_TRACE(base);
			SCOPED_TRACE(exp);
			if (std::isnan(std::pow(base, exp))) { continue; }
			ccm::test::ExpectUlpBinaryVsStd(base, exp, ccm::pow<double>, static_cast<double (*)(double, double)>(std::pow));
		}
	}
}

TEST(CcmathPowerUlpTests, PowFloat)
{
	for (std::size_t i = 0; i < ccm::test::samples::kPowFloatPairsBase.size(); ++i)
	{
		const float base = ccm::test::samples::kPowFloatPairsBase[i];
		const float exp	 = ccm::test::samples::kPowFloatPairsExp[i];
		SCOPED_TRACE(base);
		SCOPED_TRACE(exp);
		if (std::isnan(std::pow(base, exp))) { continue; }
		ccm::test::ExpectUlpBinaryVsStd(base, exp, ccm::pow<float>, static_cast<float (*)(float, float)>(std::pow));
	}
}

TEST(CcmathPowerUlpTests, PowNegativeBaseIntegerExponent)
{
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(-4.0, 2.0), std::pow(-4.0, 2.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(-4.0, 3.0), std::pow(-4.0, 3.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(-2.0, 4.0), std::pow(-2.0, 4.0));
	ccm::test::ExpectDomainEdgeMatchesStd(-4.0, [](double b) { return ccm::pow(b, 0.5); }, [](double b) { return std::pow(b, 0.5); });
	ccm::test::ExpectDomainEdgeMatchesStd(-4.0, [](double b) { return ccm::pow(b, 1.5); }, [](double b) { return std::pow(b, 1.5); });
}

TEST(CcmathPowerUlpTests, PowSpecialCasesMatchLibm)
{
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(1.0, 0.0), std::pow(1.0, 0.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(2.0, 0.0), std::pow(2.0, 0.0));
	ccm::test::ExpectDomainEdgeMatchesStd(std::numeric_limits<double>::quiet_NaN(),
										  [](double x) { return ccm::pow(x, 1.0); },
										  [](double x) { return std::pow(x, 1.0); });
	ccm::test::ExpectDomainEdgeMatchesStd(2.0, [](double x) { return ccm::pow(x, std::numeric_limits<double>::quiet_NaN()); },
										  [](double x) { return std::pow(x, std::numeric_limits<double>::quiet_NaN()); });
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(0.0, 2.0), std::pow(0.0, 2.0));
	ccm::test::ExpectSameFloatingAsStd(ccm::pow(0.0, -1.0), std::pow(0.0, -1.0));
}

TEST(CcmathPowerUlpTests, CbrtDouble)
{
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kCbrtDouble, ccm::cbrt<double>, static_cast<double (*)(double)>(std::cbrt));
}

TEST(CcmathPowerUlpTests, CbrtFloat)
{
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kCbrtFloat, ccm::cbrt<float>, static_cast<float (*)(float)>(std::cbrt));
}

TEST(CcmathPowerUlpTests, HypotDouble)
{
	for (std::size_t i = 0; i < ccm::test::samples::kHypotPairXDouble.size(); ++i)
	{
		SCOPED_TRACE(ccm::test::samples::kHypotPairXDouble[i]);
		SCOPED_TRACE(ccm::test::samples::kHypotPairYDouble[i]);
		ccm::test::ExpectUlpBinaryVsStd(ccm::test::samples::kHypotPairXDouble[i], ccm::test::samples::kHypotPairYDouble[i], ccm::hypot<double>,
										static_cast<double (*)(double, double)>(std::hypot));
	}
}

TEST(CcmathPowerUlpTests, HypotFloat)
{
	for (std::size_t i = 0; i < ccm::test::samples::kHypotPairXFloat.size(); ++i)
	{
		SCOPED_TRACE(ccm::test::samples::kHypotPairXFloat[i]);
		SCOPED_TRACE(ccm::test::samples::kHypotPairYFloat[i]);
		ccm::test::ExpectUlpBinaryVsStd(ccm::test::samples::kHypotPairXFloat[i], ccm::test::samples::kHypotPairYFloat[i], ccm::hypot<float>,
										static_cast<float (*)(float, float)>(std::hypot));
	}
}
