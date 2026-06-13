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

namespace
{
	template <typename T, typename CcmFn, typename StdFn>
	void ExpectTanAwayFromPole(T input, CcmFn ccm_fn, StdFn std_fn, T pole_guard)
	{
		if (std::abs(std::cos(input)) < pole_guard) { return; }
		ccm::test::ExpectUlpUnaryVsStd(input, ccm_fn, std_fn);
	}

	template <typename T, typename CcmFn, typename StdFn, std::size_t N>
	void ExpectTanOver(const std::array<T, N>& inputs, CcmFn ccm_fn, StdFn std_fn, T pole_guard)
	{
		for (T input : inputs)
		{
			SCOPED_TRACE(input);
			ExpectTanAwayFromPole(input, ccm_fn, std_fn, pole_guard);
		}
	}
} // namespace

TEST(CcmathTrigonometricUlpTests, SinFloat)
{ ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kTrigFloat, ccm::sin<float>, static_cast<float (*)(float)>(std::sin)); }

TEST(CcmathTrigonometricUlpTests, CosFloat)
{ ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kTrigFloat, ccm::cos<float>, static_cast<float (*)(float)>(std::cos)); }

TEST(CcmathTrigonometricUlpTests, TanFloat)
{ ExpectTanOver(ccm::test::samples::kTrigFloat, ccm::tan<float>, static_cast<float (*)(float)>(std::tan), 1e-6F); }

TEST(CcmathTrigonometricUlpTests, SinDouble)
{ ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kTrigDouble, ccm::sin<double>, static_cast<double (*)(double)>(std::sin)); }

TEST(CcmathTrigonometricUlpTests, CosDouble)
{ ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kTrigDouble, ccm::cos<double>, static_cast<double (*)(double)>(std::cos)); }

TEST(CcmathTrigonometricUlpTests, TanDouble)
{ ExpectTanOver(ccm::test::samples::kTrigDouble, ccm::tan<double>, static_cast<double (*)(double)>(std::tan), 1e-12); }

TEST(CcmathTrigonometricUlpTests, AsinFloat)
{ ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kInvTrigUnitFloat, ccm::asin<float>, static_cast<float (*)(float)>(std::asin)); }

TEST(CcmathTrigonometricUlpTests, AcosFloat)
{ ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kInvTrigUnitFloat, ccm::acos<float>, static_cast<float (*)(float)>(std::acos)); }

TEST(CcmathTrigonometricUlpTests, AsinDouble)
{ ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kInvTrigUnitDouble, ccm::asin<double>, static_cast<double (*)(double)>(std::asin)); }

TEST(CcmathTrigonometricUlpTests, AcosDouble)
{ ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kInvTrigUnitDouble, ccm::acos<double>, static_cast<double (*)(double)>(std::acos)); }

TEST(CcmathTrigonometricUlpTests, AtanDouble)
{ ccm::test::ExpectUlpUnaryOver(ccm::test::samples::kAtanDouble, ccm::atan<double>, static_cast<double (*)(double)>(std::atan)); }
