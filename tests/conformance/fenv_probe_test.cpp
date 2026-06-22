/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "ccmath/internal/support/fenv/rounding_mode.hpp"
#include "utils/fenv_fixture.hpp"

#include <gtest/gtest.h>

TEST(CcmathFenvProbeTests, StdFenvSupportsAllRoundingModes)
{
	if (!ccm::test::FenvIsSupported()) { GTEST_SKIP() << "platform does not support all IEEE rounding modes"; }

	for (int mode : ccm::test::kStdRoundingModes)
	{
		SCOPED_TRACE(ccm::test::RoundingModeName(mode));
		ccm::test::ScopedRoundingMode scope(mode);
		ASSERT_TRUE(scope.active());
		EXPECT_EQ(std::fegetround(), mode);
	}
}

TEST(CcmathFenvProbeTests, CcmGetRoundingModeTracksStdFenv)
{
	if (!ccm::test::FenvIsSupported()) { GTEST_SKIP() << "platform does not support all IEEE rounding modes"; }

	for (int mode : ccm::test::kStdRoundingModes)
	{
		SCOPED_TRACE(ccm::test::RoundingModeName(mode));
		ccm::test::ScopedRoundingMode scope(mode);
		ASSERT_TRUE(scope.active());
		EXPECT_EQ(ccm::support::fenv::get_rounding_mode(), mode);
	}
}
