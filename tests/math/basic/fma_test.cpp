/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "utils/fenv_fixture.hpp"
#include "utils/fp_matcher.hpp"
#include "utils/math_samples.hpp"
#include "utils/std_compare.hpp"
#include "utils/test_runtime.hpp"

#include <gtest/gtest.h>

#include <ccmath/ccmath.hpp>
#include <ccmath/internal/support/bits.hpp>
#include <ccmath/internal/support/fp/fma.hpp>

#include <cfenv>
#include <cmath>
#include <cstdint>

namespace
{
	template <typename T>
	constexpr bool same_bits(T lhs, T rhs)
	{
		if constexpr (std::is_same_v<T, float>) { return ccm::support::bit_cast<std::uint32_t>(lhs) == ccm::support::bit_cast<std::uint32_t>(rhs); }
		else if constexpr (std::is_same_v<T, double>) { return ccm::support::bit_cast<std::uint64_t>(lhs) == ccm::support::bit_cast<std::uint64_t>(rhs); }
		else { return lhs == rhs; }
	}

	constexpr float kFloatResidual =
		ccm::fmaf(0x1.fffep-1F, 0x1.fffep-1F, -(0x1.fffep-1F * 0x1.fffep-1F));
	constexpr double kDoubleResidual =
		ccm::fma(0x1.ffffffffffep-1, 0x1.ffffffffffep-1, -(0x1.ffffffffffep-1 * 0x1.ffffffffffep-1));

	static_assert(ccm::fma(1, 2, 3) == 5, "ccm::fma must be usable in constant evaluation");
	static_assert(same_bits(kFloatResidual, 0x1p-32F), "constexpr ccm::fmaf must preserve fused residuals");
	static_assert(same_bits(kDoubleResidual, 0x1p-88), "constexpr ccm::fma must preserve fused residuals");

#if CCM_CONSTEXPR_ROUNDING_MODE == FE_TONEAREST
	// Double-rounding killer: (double)x*(double)y + (double)z cast back to float rounds twice and
	// returns 0x3f801000, which is 1 ulp below the correctly-rounded fused result 0x3f801001.
	// These guard the constexpr paths (which cannot use a runtime builtin) against regressing to a
	// naive double evaluation. See include/ccmath/internal/support/fp/fma.hpp.
	inline constexpr float kKillerX = 0x1.001p+0F; // 1 + 2^-12
	inline constexpr float kKillerZ = 0x1.0p-53F;
	inline constexpr std::uint32_t kKillerCorrect = 0x3f801001U;

	static_assert(ccm::support::bit_cast<std::uint32_t>(ccm::fmaf(kKillerX, kKillerX, kKillerZ)) == kKillerCorrect,
				  "constexpr ccm::fmaf must round the fused result once, not twice");
	static_assert(
		ccm::support::bit_cast<std::uint32_t>(ccm::support::fp::public_fma<float>(kKillerX, kKillerX, kKillerZ)) == kKillerCorrect,
		"constexpr public_fma<float> must round the fused result once, not twice");
	static_assert(
		ccm::support::bit_cast<std::uint32_t>(ccm::support::fp::generic_fma<float>(kKillerX, kKillerX, kKillerZ)) == kKillerCorrect,
		"constexpr generic_fma<float> must round the fused result once, not twice");
	static_assert(
		ccm::support::bit_cast<std::uint32_t>(ccm::support::fp::fma_internal::fixed_fma<float, true>(kKillerX, kKillerX, kKillerZ)) ==
			kKillerCorrect,
		"constexpr fixed_fma<float> must round the fused result once, not twice");
#endif

	using ccm::test::runtime_value;
} // namespace

TEST(CcmathBasicTests, FmaMatchesStdStructuredFloatCases)
{
	for (const auto & input : ccm::test::samples::kFmaFloatCases)
	{
		ccm::test::ExpectTernaryMatchesStd(
			input.x, input.y, input.z, ccm::fmaf, static_cast<float (*)(float, float, float)>(std::fma));
	}
}

TEST(CcmathBasicTests, FmaMatchesStdStructuredDoubleCases)
{
	for (const auto & input : ccm::test::samples::kFmaDoubleCases)
	{
		ccm::test::ExpectTernaryMatchesStd(
			input.x, input.y, input.z, ccm::fma<double>, static_cast<double (*)(double, double, double)>(std::fma));
	}
}

TEST(CcmathBasicTests, FmaMatchesStdLongDoubleSmokeCases)
{
	ccm::test::ExpectTernaryMatchesStd(
		1.0L, 2.0L, 3.0L, ccm::fmal, static_cast<long double (*)(long double, long double, long double)>(std::fma));
	ccm::test::ExpectTernaryMatchesStd(
		0x1.fffffffffffffp-1L,
		0x1.fffffffffffffp-1L,
		-0x1.ep-2L,
		ccm::fmal,
		static_cast<long double (*)(long double, long double, long double)>(std::fma));
}

// The internal quiet (generic_fma) and signaling (public_fma) software kernels are the paths
// reached by error-free transforms and by the runtime fallback in directed rounding modes. They are
// not exercised through ccm::fma in round-to-nearest (which may use the native builtin), so test
// them directly. The double-rounding killers must be correctly rounded in every mode.
TEST(CcmathBasicTests, FmaInternalSoftwarePathsAvoidDoubleRoundingAllModes)
{
	for (const auto & input : ccm::test::samples::kFmaFloatCases)
	{
		SCOPED_TRACE(input.x);
		SCOPED_TRACE(input.y);
		SCOPED_TRACE(input.z);
		for (const int mode : ccm::test::kStdRoundingModes)
		{
			SCOPED_TRACE(ccm::test::RoundingModeName(mode));
			ccm::test::ForceRoundingMode force(mode);
			ASSERT_TRUE(force);

			const float x = runtime_value(input.x);
			const float y = runtime_value(input.y);
			const float z = runtime_value(input.z);
			const float reference = std::fmaf(x, y, z);

			ccm::test::ExpectFpEq(ccm::support::fp::public_fma<float>(x, y, z), reference);
			ccm::test::ExpectFpEq(ccm::support::fp::generic_fma<float>(x, y, z), reference);
		}
	}

	for (const auto & input : ccm::test::samples::kFmaDoubleCases)
	{
		SCOPED_TRACE(input.x);
		SCOPED_TRACE(input.y);
		SCOPED_TRACE(input.z);
		for (const int mode : ccm::test::kStdRoundingModes)
		{
			SCOPED_TRACE(ccm::test::RoundingModeName(mode));
			ccm::test::ForceRoundingMode force(mode);
			ASSERT_TRUE(force);

			const double x = runtime_value(input.x);
			const double y = runtime_value(input.y);
			const double z = runtime_value(input.z);
			const double reference = std::fma(x, y, z);

			ccm::test::ExpectFpEq(ccm::support::fp::public_fma<double>(x, y, z), reference);
			ccm::test::ExpectFpEq(ccm::support::fp::generic_fma<double>(x, y, z), reference);
		}
	}
}
