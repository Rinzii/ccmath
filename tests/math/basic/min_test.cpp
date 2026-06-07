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

#include <ccmath/ccmath.hpp>
#include <cmath>
#include <limits>

#include "utils/std_compare.hpp"


TEST(CcmathBasicTests, Min)
{
	// Verify that ccm::min works with static_assert
	static_assert(ccm::min(1, 2) == 1, "min has failed testing that it is static_assert-able!");

	// Test the ccm::abs function against std::abs
	ccm::test::ExpectSameAsStd(ccm::min(1, 2), std::min(1, 2));
	ccm::test::ExpectSameAsStd(ccm::min(2, 1), std::min(2, 1));
	ccm::test::ExpectSameAsStd(ccm::min(1, 1), std::min(1, 1));
	ccm::test::ExpectSameAsStd(ccm::min(0, 0), std::min(0, 0));
	ccm::test::ExpectSameAsStd(ccm::min(-1, 0), std::min(-1, 0));
	ccm::test::ExpectSameAsStd(ccm::min(0, -1), std::min(0, -1));
	ccm::test::ExpectSameAsStd(ccm::min(-1, -1), std::min(-1, -1));

	// now floats
	ccm::test::ExpectSameAsStd(ccm::min(1.0F, 2.0F), std::min(1.0F, 2.0F));
	ccm::test::ExpectSameAsStd(ccm::min(2.0F, 1.0F), std::min(2.0F, 1.0F));
	ccm::test::ExpectSameAsStd(ccm::min(1.0F, 1.0F), std::min(1.0F, 1.0F));
	ccm::test::ExpectSameAsStd(ccm::min(0.0F, 0.0F), std::min(0.0F, 0.0F));
	ccm::test::ExpectSameAsStd(ccm::min(-1.0F, 0.0F), std::min(-1.0F, 0.0F));
	ccm::test::ExpectSameAsStd(ccm::min(0.0F, -1.0F), std::min(0.0F, -1.0F));
	ccm::test::ExpectSameAsStd(ccm::min(-1.0F, -1.0F), std::min(-1.0F, -1.0F));

	// Mixed integer and floating-point arguments.
	ccm::test::ExpectSameAsStd(ccm::fmin(1, 2.0F), std::fmin(1, 2.0F));
	ccm::test::ExpectSameAsStd(ccm::fmin(2.0F, 1), std::fmin(2.0F, 1));
	ccm::test::ExpectSameAsStd(ccm::fmin(1, 1.0F), std::fmin(1, 1.0F));
	ccm::test::ExpectSameAsStd(ccm::fmin(0.0F, 0), std::fmin(0.0F, 0));
	ccm::test::ExpectSameAsStd(ccm::fmin(-1, 0.0F), std::fmin(-1, 0.0F));
	ccm::test::ExpectSameAsStd(ccm::fmin(0.0F, -1), std::fmin(0.0F, -1));
	ccm::test::ExpectSameAsStd(ccm::fmin(-1.0F, -1), std::fmin(-1.0F, -1));

}
