/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "utils/conformance_suite.hpp"
#include "utils/test_runtime.hpp"

#include <gtest/gtest.h>

#include <ccmath/ccmath.hpp>
#include <ccmath/internal/support/fp/fma.hpp>

#include <cmath>
#include <limits>

#if defined(_MSC_VER) && !defined(__clang__)
	#define CCMATH_SKIP_MSVC_FENV_EXCEPTIONS() GTEST_SKIP() << "fenv exception flags are not reliable under MSVC"
#else
	#define CCMATH_SKIP_MSVC_FENV_EXCEPTIONS() static_cast<void>(0)
#endif

namespace
{
	using ccm::test::runtime_value;

	template <typename T> void consume(T value)
	{
		[[maybe_unused]] volatile T sink = value;
	}
} // namespace

TEST(CcmathSoftwareFmaFenvTests, InvalidFlagsMatchStd)
{
	CCMATH_SKIP_MSVC_FENV_EXCEPTIONS();

	ccm::test::ExpectFenvFlagsMatchStd(
		[] { consume(ccm::fmaf(runtime_value(0.0F), runtime_value(std::numeric_limits<float>::infinity()), runtime_value(1.0F))); },
		[] { consume(std::fmaf(runtime_value(0.0F), runtime_value(std::numeric_limits<float>::infinity()), runtime_value(1.0F))); },
		FE_INVALID);

	ccm::test::ExpectFenvFlagsMatchStd(
		[] { consume(ccm::fma(runtime_value(0.0), runtime_value(std::numeric_limits<double>::infinity()), runtime_value(1.0))); },
		[] { consume(std::fma(runtime_value(0.0), runtime_value(std::numeric_limits<double>::infinity()), runtime_value(1.0))); },
		FE_INVALID);
}

TEST(CcmathSoftwareFmaFenvTests, InvalidFlagWithQuietNanAddendMatchesStd)
{
	CCMATH_SKIP_MSVC_FENV_EXCEPTIONS();

	// IEEE-754: 0*inf (or inf*0) is an invalid operation even when the addend is a quiet NaN.
	// The software fma path must signal FE_INVALID here, matching std::fma and the native fma.
	const float inf_f  = std::numeric_limits<float>::infinity();
	const float nan_f  = std::numeric_limits<float>::quiet_NaN();
	const double inf_d = std::numeric_limits<double>::infinity();
	const double nan_d = std::numeric_limits<double>::quiet_NaN();

	ccm::test::ExpectFenvFlagsMatchStd([&] { consume(ccm::support::fp::public_fma<float>(runtime_value(0.0F), runtime_value(inf_f), runtime_value(nan_f))); },
									   [&] { consume(std::fmaf(runtime_value(0.0F), runtime_value(inf_f), runtime_value(nan_f))); },
									   FE_INVALID);

	ccm::test::ExpectFenvFlagsMatchStd([&] { consume(ccm::support::fp::public_fma<double>(runtime_value(inf_d), runtime_value(0.0), runtime_value(nan_d))); },
									   [&] { consume(std::fma(runtime_value(inf_d), runtime_value(0.0), runtime_value(nan_d))); },
									   FE_INVALID);
}

TEST(CcmathSoftwareFmaFenvTests, RangeFlagsMatchStdAllModes)
{
	CCMATH_SKIP_MSVC_FENV_EXCEPTIONS();

	ccm::test::ForEachRoundingModeOrSkip([&](int mode) {
		ccm::test::ForceRoundingMode force(mode);
		ASSERT_TRUE(force);

		ccm::test::ExpectFenvFlagsMatchStd(
			[] { consume(ccm::fmaf(runtime_value(std::numeric_limits<float>::max()), runtime_value(2.0F), runtime_value(std::numeric_limits<float>::max()))); },
			[] { consume(std::fmaf(runtime_value(std::numeric_limits<float>::max()), runtime_value(2.0F), runtime_value(std::numeric_limits<float>::max()))); },
			FE_OVERFLOW | FE_INEXACT);

		ccm::test::ExpectFenvFlagsMatchStd(
			[] { consume(ccm::fmaf(runtime_value(std::numeric_limits<float>::denorm_min()), runtime_value(0.5F), runtime_value(0.0F))); },
			[] { consume(std::fmaf(runtime_value(std::numeric_limits<float>::denorm_min()), runtime_value(0.5F), runtime_value(0.0F))); },
			FE_UNDERFLOW | FE_INEXACT);

		ccm::test::ExpectFenvFlagsMatchStd(
			[] { consume(ccm::fma(runtime_value(std::numeric_limits<double>::max()), runtime_value(2.0), runtime_value(std::numeric_limits<double>::max()))); },
			[] { consume(std::fma(runtime_value(std::numeric_limits<double>::max()), runtime_value(2.0), runtime_value(std::numeric_limits<double>::max()))); },
			FE_OVERFLOW | FE_INEXACT);

		ccm::test::ExpectFenvFlagsMatchStd(
			[] { consume(ccm::fma(runtime_value(std::numeric_limits<double>::denorm_min()), runtime_value(0.5), runtime_value(0.0))); },
			[] { consume(std::fma(runtime_value(std::numeric_limits<double>::denorm_min()), runtime_value(0.5), runtime_value(0.0))); },
			FE_UNDERFLOW | FE_INEXACT);
	});
}

TEST(CcmathSoftwareFmaFenvTests, InexactFlagsMatchStdAllModes)
{
	CCMATH_SKIP_MSVC_FENV_EXCEPTIONS();

	ccm::test::ForEachRoundingModeOrSkip([&](int mode) {
		ccm::test::ForceRoundingMode force(mode);
		ASSERT_TRUE(force);

		ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::fmaf(runtime_value(0.1F), runtime_value(0.1F), runtime_value(0.0F))); },
										   [] { consume(std::fmaf(runtime_value(0.1F), runtime_value(0.1F), runtime_value(0.0F))); },
										   FE_INEXACT);

		ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::fma(runtime_value(0.1), runtime_value(0.1), runtime_value(0.0))); },
										   [] { consume(std::fma(runtime_value(0.1), runtime_value(0.1), runtime_value(0.0))); },
										   FE_INEXACT);
	});
}

#undef CCMATH_SKIP_MSVC_FENV_EXCEPTIONS
