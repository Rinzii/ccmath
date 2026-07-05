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
#include "utils/math_samples.hpp"
#include "utils/ulp_suite.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>

TEST(CcmathExponentialUlpTests, ExpDouble)
{
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kExpoDouble, ccm::exp<double>, static_cast<double (*)(double)>(std::exp));
}

TEST(CcmathExponentialUlpTests, Exp2Double)
{
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kExpoDouble, ccm::exp2<double>, static_cast<double (*)(double)>(std::exp2));
}

TEST(CcmathExponentialUlpTests, LogDouble)
{
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kExpoDouble, ccm::log<double>, static_cast<double (*)(double)>(std::log));
}

TEST(CcmathExponentialUlpTests, Log2Double)
{
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kExpoDouble, ccm::log2<double>, static_cast<double (*)(double)>(std::log2));
}

TEST(CcmathExponentialUlpTests, Log10Double)
{
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kExpoDouble, ccm::log10<double>, static_cast<double (*)(double)>(std::log10));
}

TEST(CcmathExponentialUlpTests, ExpFloat)
{
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kExpoFloat, ccm::exp<float>, static_cast<float (*)(float)>(std::exp));
}

TEST(CcmathExponentialUlpTests, Exp2Float)
{
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kExpoFloat, ccm::exp2<float>, static_cast<float (*)(float)>(std::exp2));
}

TEST(CcmathExponentialUlpTests, LogFloat)
{
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kExpoFloat, ccm::log<float>, static_cast<float (*)(float)>(std::log));
}

TEST(CcmathExponentialUlpTests, Log2Float)
{
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kExpoFloat, ccm::log2<float>, static_cast<float (*)(float)>(std::log2));
}

TEST(CcmathExponentialUlpTests, Log10Float)
{
	ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kExpoFloat, ccm::log10<float>, static_cast<float (*)(float)>(std::log10));
}

TEST(CcmathExponentialUlpTests, DomainEdgeCasesMatchLibm)
{
	ccm::test::ExpectDomainEdgeMatchesStd(0.0, ccm::exp<double>, static_cast<double (*)(double)>(std::exp));
	ccm::test::ExpectDomainEdgeMatchesStd(-0.0, ccm::exp<double>, static_cast<double (*)(double)>(std::exp));
	ccm::test::ExpectDomainEdgeMatchesStd(std::numeric_limits<double>::infinity(), ccm::exp<double>, static_cast<double (*)(double)>(std::exp));
	ccm::test::ExpectDomainEdgeMatchesStd(-std::numeric_limits<double>::infinity(), ccm::exp<double>, static_cast<double (*)(double)>(std::exp));
	ccm::test::ExpectDomainEdgeMatchesStd(std::numeric_limits<double>::quiet_NaN(), ccm::exp<double>, static_cast<double (*)(double)>(std::exp));

	ccm::test::ExpectDomainEdgeMatchesStd(1.0, ccm::log<double>, static_cast<double (*)(double)>(std::log));
	ccm::test::ExpectDomainEdgeMatchesStd(0.0, ccm::log<double>, static_cast<double (*)(double)>(std::log));
	ccm::test::ExpectDomainEdgeMatchesStd(std::numeric_limits<double>::quiet_NaN(), ccm::log<double>, static_cast<double (*)(double)>(std::log));
	ccm::test::ExpectCcmNegativeDomainNaN(-1.0, ccm::log<double>);

	ccm::test::ExpectDomainEdgeMatchesStd(1.0, ccm::log10<double>, static_cast<double (*)(double)>(std::log10));
	ccm::test::ExpectDomainEdgeMatchesStd(10.0, ccm::log10<double>, static_cast<double (*)(double)>(std::log10));
	ccm::test::ExpectDomainEdgeMatchesStd(0.0, ccm::log10<double>, static_cast<double (*)(double)>(std::log10));
	ccm::test::ExpectDomainEdgeMatchesStd(std::numeric_limits<double>::quiet_NaN(), ccm::log10<double>, static_cast<double (*)(double)>(std::log10));
	ccm::test::ExpectDomainEdgeMatchesStd(std::numeric_limits<double>::infinity(), ccm::log10<double>, static_cast<double (*)(double)>(std::log10));
	ccm::test::ExpectCcmNegativeDomainNaN(-1.0, ccm::log10<double>);
}
