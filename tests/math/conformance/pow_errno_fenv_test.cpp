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
#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"
#include "ccmath/internal/predef/attributes/never_inline.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "utils/conformance_suite.hpp"
#include "utils/test_runtime.hpp"

#include <gtest/gtest.h>

#include <array>
#include <cerrno>
#include <cmath>
#include <limits>

namespace
{
	using ccm::test::runtime_value;

	template <typename T>
	void consume(T value)
	{
		volatile T sink = value;
		(void)sink;
	}

	CCM_NEVER_INLINE double invoke_pow_gen(double base, double exp)
	{
		return ccm::gen::pow_gen(base, exp);
	}

	CCM_NEVER_INLINE float invoke_powf_gen(float base, float exp)
	{
		return ccm::gen::pow_gen(base, exp);
	}
} // namespace

TEST(CcmathPowErrnoTests, GenericPowSetsErrnoWhenRuntimeErrnoIsEnabled)
{
	if constexpr ((ccm::support::fenv::ccm_math_err_handling() & ccm::support::fenv::get_mode(ccm::support::fenv::ccm_math_err_mode::eErrno)) == 0)
	{
		GTEST_SKIP() << "this build does not advertise errno-setting math error handling";
	}

	errno = 0;
	{
		volatile double base = -1.0;
		volatile double exp	 = 0.5;
		(void)invoke_pow_gen(base, exp);
	}
	EXPECT_EQ(errno, EDOM) << "double negative base domain error";

	errno = 0;
	{
		volatile float base = -1.0F;
		volatile float exp	= 0.5F;
		(void)invoke_powf_gen(base, exp);
	}
	EXPECT_EQ(errno, EDOM) << "float negative base domain error";

	errno = 0;
	{
		volatile double base = 0.0;
		volatile double exp	 = -1.0;
		(void)invoke_pow_gen(base, exp);
	}
	EXPECT_EQ(errno, EDOM) << "double pole error";

	errno = 0;
	{
		volatile float base = 0.0F;
		volatile float exp	= -1.0F;
		(void)invoke_powf_gen(base, exp);
	}
	EXPECT_EQ(errno, EDOM) << "float pole error";
}

TEST(CcmathPowFenvTests, OverflowAndUnderflowFlagsMatchStdForPowPublicApi)
{
	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::pow(runtime_value(std::numeric_limits<double>::max()), runtime_value(2.0))); },
									   [] { consume(std::pow(runtime_value(std::numeric_limits<double>::max()), runtime_value(2.0))); },
									   FE_OVERFLOW);

	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::pow(runtime_value(2.0), runtime_value(-1075.0))); },
									   [] { consume(std::pow(runtime_value(2.0), runtime_value(-1075.0))); },
									   FE_UNDERFLOW);

	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::powf(runtime_value(std::numeric_limits<float>::max()), runtime_value(2.0F))); },
									   [] { consume(std::pow(runtime_value(std::numeric_limits<float>::max()), runtime_value(2.0F))); },
									   FE_OVERFLOW);

	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::powf(runtime_value(2.0F), runtime_value(-150.0F))); },
									   [] { consume(std::pow(runtime_value(2.0F), runtime_value(-150.0F))); },
									   FE_UNDERFLOW);
}

TEST(CcmathPowFenvTests, GenericPowfBaseTwoMatchesStdAtRangeThresholdsAllModes)
{
	constexpr std::array<float, 5> kExponents = { 128.0F, 129.0F, -150.0F, -151.0F, -152.0F };

	ccm::test::ForEachRoundingModeOrSkip(
		[&](int mode)
		{
			ccm::test::ScopedRoundingMode scope(mode);
			ASSERT_TRUE(scope.active());

			for (float exponent_value : kExponents)
			{
				SCOPED_TRACE(exponent_value);

				const float base	   = runtime_value(2.0F);
				const float exponent = runtime_value(exponent_value);
				const float actual   = invoke_powf_gen(base, exponent);
				const float expected = static_cast<float>(std::pow(base, exponent));

				ccm::test::ExpectFpEq(actual, expected);
			}
		});
}

TEST(CcmathPowFenvTests, SignalingNaNsRaiseInvalidLikeStdWhenObservable)
{
	if constexpr (!std::numeric_limits<double>::has_signaling_NaN || !std::numeric_limits<float>::has_signaling_NaN)
	{
		GTEST_SKIP() << "signaling NaNs are not available on this platform";
	}

	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::pow(runtime_value(std::numeric_limits<double>::signaling_NaN()), runtime_value(2.0))); },
									   [] { consume(std::pow(runtime_value(std::numeric_limits<double>::signaling_NaN()), runtime_value(2.0))); },
									   FE_INVALID);

	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::pow(runtime_value(2.0), runtime_value(std::numeric_limits<double>::signaling_NaN()))); },
									   [] { consume(std::pow(runtime_value(2.0), runtime_value(std::numeric_limits<double>::signaling_NaN()))); },
									   FE_INVALID);

	ccm::test::ExpectFenvFlagsMatchStd([] { consume(ccm::powf(runtime_value(std::numeric_limits<float>::signaling_NaN()), runtime_value(2.0F))); },
									   [] { consume(std::pow(runtime_value(std::numeric_limits<float>::signaling_NaN()), runtime_value(2.0F))); },
									   FE_INVALID);
}

TEST(CcmathPowExceptionalValueTests, QuietAndSignalingNaNsProduceNaNResults)
{
	EXPECT_TRUE(std::isnan(ccm::pow(std::numeric_limits<double>::quiet_NaN(), 2.0)));
	EXPECT_TRUE(std::isnan(ccm::pow(2.0, std::numeric_limits<double>::quiet_NaN())));
	EXPECT_TRUE(std::isnan(ccm::powf(std::numeric_limits<float>::quiet_NaN(), 2.0F)));
	EXPECT_TRUE(std::isnan(ccm::powf(2.0F, std::numeric_limits<float>::quiet_NaN())));

	if constexpr (std::numeric_limits<double>::has_signaling_NaN)
	{
		EXPECT_TRUE(std::isnan(ccm::pow(std::numeric_limits<double>::signaling_NaN(), 2.0)));
		EXPECT_TRUE(std::isnan(ccm::pow(2.0, std::numeric_limits<double>::signaling_NaN())));
	}

	if constexpr (std::numeric_limits<float>::has_signaling_NaN)
	{
		EXPECT_TRUE(std::isnan(ccm::powf(std::numeric_limits<float>::signaling_NaN(), 2.0F)));
		EXPECT_TRUE(std::isnan(ccm::powf(2.0F, std::numeric_limits<float>::signaling_NaN())));
	}
}
