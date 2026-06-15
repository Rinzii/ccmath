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
#include "ccmath/internal/math/generic/func/expo/exp2_gen.hpp"
#include "ccmath/internal/math/generic/func/expo/exp_gen.hpp"
#include "ccmath/internal/math/generic/func/expo/log10_gen.hpp"
#include "ccmath/internal/math/generic/func/expo/log2_gen.hpp"
#include "ccmath/internal/math/generic/func/expo/log_gen.hpp"
// TODO(IanP): re-enable alongside the log1p / expm1 directed-mode tests below once their generic
// kernels meet the accuracy contract.
// #include "ccmath/internal/math/generic/func/expo/expm1_gen.hpp"
// #include "ccmath/internal/math/generic/func/expo/log1p_gen.hpp"
#include "utils/conformance_suite.hpp"
#include "utils/math_samples.hpp"
#include "utils/test_runtime.hpp"

#include <gtest/gtest.h>

#include <array>
#include <cmath>

namespace
{
	using ccm::test::runtime_value;

	// Normal positives that exercise every log table bucket with full-width significands, where
	// the host libm is most likely to disagree with the correctly rounded result under a directed
	// mode. Valid inputs for log, log2 and log10.
	constexpr std::array<double, 9> kLogDirectedProbes = {
		0x1.5fdffffffff32p-1, 0x1.921fb54442d18p+0, 0x1.5bf0a8b145769p+1, 0x1.62e42fefa39efp+2,	 0x1.b7e151628aed3p+4,
		0x1.0000000000001p+0, 0x1.fffffffffffffp+9, 0x1.199999999999ap-7, 0x1.5fdffffffff32p+40,
	};

	// TODO(IanP): re-enable with the log1p directed-mode test once the generic log1p kernel meets the
	// accuracy contract. log1p domain is (-1, inf): small negatives near the lower edge plus
	// full-width positives.
	/*
	constexpr std::array<double, 9> kLog1pDirectedProbes = {
		-0x1.eb851eb851eb8p-1, -0x1.999999999999ap-2, -0x1.0000000000000p-10, 0x1.199999999999ap-7, 0x1.5fdffffffff32p-1,
		0x1.921fb54442d18p+0,  0x1.62e42fefa39efp+2,  0x1.b7e151628aed3p+4,	 0x1.5fdffffffff32p+10,
	};
	*/

	// Finite magnitudes for exp and exp2 that stay well clear of overflow and underflow so the
	// result is a full-precision normal value rather than a saturated edge.
	constexpr std::array<double, 10> kExpDirectedProbes = {
		-0x1.62e42fefa39efp+5, -0x1.5fdffffffff32p+4, -0x1.921fb54442d18p+0, -0x1.199999999999ap-7, 0x1.199999999999ap-7,
		0x1.5bf0a8b145769p+1,  0x1.921fb54442d18p+0,  0x1.5fdffffffff32p+4,	 0x1.b7e151628aed3p+2,	0x1.62e42fefa39efp+5,
	};

	// The expo runtime headers route to the generic kernel outside FE_TONEAREST, because the libm
	// builtin (and the MSVC libm fallback) is only correctly rounded under round to nearest. This
	// pins that contract: under each directed mode the runtime dispatch must reproduce the generic
	// kernel bit for bit. Drop the guard from any *_rt.hpp and these diverge wherever the host libm
	// is not correctly rounded. Both sides run under the forced mode through a runtime value so the
	// generic kernel reads the dynamic mode instead of folding at compile time.
	template <typename CcmFn, typename GenFn, std::size_t N>
	void ExpectRuntimeMatchesGenericInDirectedModes(const std::array<double, N>& inputs, CcmFn ccm_fn, GenFn gen_fn)
	{
		ccm::test::ForEachRoundingModeOrSkip(
			[&](int mode)
			{
				if (mode == FE_TONEAREST) { return; } // round to nearest keeps the builtin, nothing to pin here
				for (double input : inputs)
				{
					SCOPED_TRACE(input);
					const double x = runtime_value(input);
					ccm::test::ExpectFpEq(ccm_fn(x), gen_fn(x));
				}
			});
	}
} // namespace

TEST(CcmathRoundingConformanceTests, NearbyIntMatchesStdAllModesDouble)
{
	ccm::test::ExpectFpUnaryOverMatchesStdAllModes(
		ccm::test::samples::kNearbyIntProbeDouble, ccm::nearbyint<double>, static_cast<double (*)(double)>(std::nearbyint));
}

TEST(CcmathRoundingConformanceTests, NearbyIntMatchesStdAllModesFloat)
{
	ccm::test::ExpectFpUnaryOverMatchesStdAllModes(
		ccm::test::samples::kNearbyIntProbeFloat, ccm::nearbyint<float>, static_cast<float (*)(float)>(std::nearbyint));
}

TEST(CcmathRoundingConformanceTests, LogOneMatchesStdAllModes)
{
	ccm::test::ForEachRoundingModeOrSkip(
		[&](int mode)
		{
			ccm::test::ScopedRoundingMode scope(mode);
			ASSERT_TRUE(scope.active());

			const double one = runtime_value(1.0);
			ccm::test::ExpectFpEq(ccm::log(one), std::log(one));
		});
}

TEST(CcmathRoundingConformanceTests, Log10OneMatchesStdAllModes)
{
	ccm::test::ForEachRoundingModeOrSkip(
		[&](int mode)
		{
			ccm::test::ScopedRoundingMode scope(mode);
			ASSERT_TRUE(scope.active());

			const double one = runtime_value(1.0);
			ccm::test::ExpectFpEq(ccm::log10(one), std::log10(one));
		});
}

TEST(CcmathRoundingConformanceTests, Log2OneMatchesStdAllModes)
{
	ccm::test::ForEachRoundingModeOrSkip(
		[&](int mode)
		{
			ccm::test::ScopedRoundingMode scope(mode);
			ASSERT_TRUE(scope.active());

			const double one = runtime_value(1.0);
			ccm::test::ExpectFpEq(ccm::log2(one), std::log2(one));
		});
}

TEST(CcmathRoundingConformanceTests, Log1pZeroMatchesStdAllModes)
{
	ccm::test::ExpectFpUnaryMatchesStdAllModes(0.0, ccm::log1p<double>, static_cast<double (*)(double)>(std::log1p));
	ccm::test::ExpectFpUnaryMatchesStdAllModes(-0.0, ccm::log1p<double>, static_cast<double (*)(double)>(std::log1p));
}

TEST(CcmathRoundingConformanceTests, Expm1ZeroMatchesStdAllModes)
{
	ccm::test::ExpectFpUnaryMatchesStdAllModes(0.0, ccm::expm1<double>, static_cast<double (*)(double)>(std::expm1));
	ccm::test::ExpectFpUnaryMatchesStdAllModes(-0.0, ccm::expm1<double>, static_cast<double (*)(double)>(std::expm1));
}

TEST(CcmathRoundingConformanceTests, ExpZeroMatchesStdAllModes)
{
	ccm::test::ExpectFpUnaryMatchesStdAllModes(0.0, ccm::exp<double>, static_cast<double (*)(double)>(std::exp));
	ccm::test::ExpectFpUnaryMatchesStdAllModes(-0.0, ccm::exp<double>, static_cast<double (*)(double)>(std::exp));
}

TEST(CcmathRoundingConformanceTests, SqrtMatchesStdAllModes)
{
	ccm::test::ExpectUlpUnaryOverAllRoundingModes(ccm::test::samples::kAllModesProbeDouble, ccm::sqrt<double>, static_cast<double (*)(double)>(std::sqrt));
	ccm::test::ExpectUlpUnaryOverAllRoundingModes(ccm::test::samples::kAllModesProbeFloat, ccm::sqrt<float>, static_cast<float (*)(float)>(std::sqrt));
}

TEST(CcmathRoundingConformanceTests, RintMatchesStdAllModesDouble)
{ ccm::test::ExpectFpUnaryOverMatchesStdAllModes(ccm::test::samples::kNearbyIntProbeDouble, ccm::rint<double>, static_cast<double (*)(double)>(std::rint)); }

TEST(CcmathRoundingConformanceTests, RintMatchesStdAllModesFloat)
{ ccm::test::ExpectFpUnaryOverMatchesStdAllModes(ccm::test::samples::kNearbyIntProbeFloat, ccm::rint<float>, static_cast<float (*)(float)>(std::rint)); }

TEST(CcmathRoundingConformanceTests, CeilTruncRoundFloorIndependentOfMode)
{
	ccm::test::ExpectFpUnaryOverMatchesStdAllModes(ccm::test::samples::kNearbyIntProbeDouble, ccm::ceil<double>, static_cast<double (*)(double)>(std::ceil));
	ccm::test::ExpectFpUnaryOverMatchesStdAllModes(ccm::test::samples::kNearbyIntProbeDouble, ccm::trunc<double>, static_cast<double (*)(double)>(std::trunc));
	ccm::test::ExpectFpUnaryOverMatchesStdAllModes(ccm::test::samples::kNearestHalfwayDouble, ccm::round<double>, static_cast<double (*)(double)>(std::round));
	ccm::test::ExpectFpUnaryOverMatchesStdAllModes(ccm::test::samples::kNearbyIntProbeDouble, ccm::floor<double>, static_cast<double (*)(double)>(std::floor));
}

// The following pin the FE_TONEAREST guard in the expo runtime headers: outside round to nearest
// the runtime dispatch must fall back to the generic kernel, so it matches the generic wrapper bit
// for bit over normal inputs.
TEST(CcmathRoundingConformanceTests, LogRuntimeMatchesGenericInDirectedModes)
{
	ExpectRuntimeMatchesGenericInDirectedModes(kLogDirectedProbes, ccm::log<double>, [](double v) { return ccm::gen::log_gen<double>(v); });
}

TEST(CcmathRoundingConformanceTests, Log2RuntimeMatchesGenericInDirectedModes)
{
	ExpectRuntimeMatchesGenericInDirectedModes(kLogDirectedProbes, ccm::log2<double>, [](double v) { return ccm::gen::log2_gen<double>(v); });
}

TEST(CcmathRoundingConformanceTests, Log10RuntimeMatchesGenericInDirectedModes)
{
	ExpectRuntimeMatchesGenericInDirectedModes(kLogDirectedProbes, ccm::log10<double>, [](double v) { return ccm::gen::log10_gen<double>(v); });
}

// TODO(IanP): re-enable once the generic log1p double kernel meets the accuracy contract. Until
// then log1p_rt keeps the libm builtin in every mode (no FE_TONEAREST guard), so the runtime path
// does not match the generic kernel under directed rounding and this would fail.
/*
TEST(CcmathRoundingConformanceTests, Log1pRuntimeMatchesGenericInDirectedModes)
{
	ExpectRuntimeMatchesGenericInDirectedModes(kLog1pDirectedProbes, ccm::log1p<double>, [](double v) { return ccm::gen::log1p_gen<double>(v); });
}
*/

TEST(CcmathRoundingConformanceTests, ExpRuntimeMatchesGenericInDirectedModes)
{
	ExpectRuntimeMatchesGenericInDirectedModes(kExpDirectedProbes, ccm::exp<double>, [](double v) { return ccm::gen::exp_gen<double>(v); });
}

TEST(CcmathRoundingConformanceTests, Exp2RuntimeMatchesGenericInDirectedModes)
{
	ExpectRuntimeMatchesGenericInDirectedModes(kExpDirectedProbes, ccm::exp2<double>, [](double v) { return ccm::gen::exp2_gen<double>(v); });
}

// TODO(IanP): re-enable once the generic expm1 double kernel meets the accuracy contract. Until
// then expm1_rt keeps the libm builtin in every mode (no FE_TONEAREST guard), so the runtime path
// does not match the generic kernel under directed rounding and this would fail.
/*
TEST(CcmathRoundingConformanceTests, Expm1RuntimeMatchesGenericInDirectedModes)
{
	ExpectRuntimeMatchesGenericInDirectedModes(kExpDirectedProbes, ccm::expm1<double>, [](double v) { return ccm::gen::expm1_gen<double>(v); });
}
*/
