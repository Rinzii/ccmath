/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <ccmath/math/nearest/rint.hpp>

#include <cfenv>
#include <cmath>
#include <functional>
#include <ranges>
#include <vector>

#include <gtest/gtest.h>

// Disabling test case ccm::nearbyintl if run on clang linux.
#ifdef __clang__
	#ifdef __linux__
		#define CLANG_LINUX
	#endif
#endif

// Compile Time Tests

#ifndef CLANG_LINUX
TEST(CcmathNearestTests, CcmRIntDoubleCanBeEvaluatedAtCompileTime)
{
	constexpr auto rint{ccm::rint(1.0)};
	static_assert(rint == 1.0);
}

TEST(CcmathNearestTests, CcmRIntFloatCanBeEvaluatedAtCompileTime)
{
	constexpr auto rint{ccm::rint(1.0f)};
	static_assert(rint == 1.0f);
}

TEST(CcmathNearestTests, CcmRIntFFloatCanBeEvaluatedAtCompileTime)
{
	constexpr auto rint{ccm::rintf(1.0f)};
	static_assert(rint == 1.0f);
}

TEST(CcmathNearestTests, CcmRIntLongDoubleCanBeEvaluatedAtCompileTime)
{
	constexpr auto rint{ccm::rint(1.0l)};
	static_assert(rint == 1.0l);
}

TEST(CcmathNearestTests, CcmRIntLLongDoubleCanBeEvaluatedAtCompileTime)
{
	constexpr auto rint{ccm::rintl(1.0l)};
	static_assert(rint == 1.0l);
}

TEST(CcmathNearestTests, CcmLRIntDoubleCanBeEvaluatedAtCompileTime)
{
	constexpr auto rint{ccm::lrint(1.0)};
	static_assert(rint == 1.0);
}

TEST(CcmathNearestTests, CcmLRIntFloatCanBeEvaluatedAtCompileTime)
{
	constexpr auto rint{ccm::lrint(1.0f)};
	static_assert(rint == 1.0f);
}

TEST(CcmathNearestTests, CcmLRIntFFloatCanBeEvaluatedAtCompileTime)
{
	constexpr auto rint{ccm::lrintf(1.0f)};
	static_assert(rint == 1.0f);
}

TEST(CcmathNearestTests, CcmLRIntLongDoubleCanBeEvaluatedAtCompileTime)
{
	constexpr auto rint{ccm::lrint(1.0l)};
	static_assert(rint == 1.0l);
}

TEST(CcmathNearestTests, CcmLRIntLLongDoubleCanBeEvaluatedAtCompileTime)
{
	constexpr auto rint{ccm::lrintl(1.0l)};
	static_assert(rint == 1.0l);
}

TEST(CcmathNearestTests, CcmLLRIntDoubleCanBeEvaluatedAtCompileTime)
{
	constexpr auto rint{ccm::llrint(1.0)};
	static_assert(rint == 1.0);
}

TEST(CcmathNearestTests, CcmLLRIntFloatCanBeEvaluatedAtCompileTime)
{
	constexpr auto rint{ccm::llrint(1.0f)};
	static_assert(rint == 1.0f);
}

TEST(CcmathNearestTests, CcmLLRIntFFloatCanBeEvaluatedAtCompileTime)
{
	constexpr auto rint{ccm::llrintf(1.0f)};
	static_assert(rint == 1.0f);
}

TEST(CcmathNearestTests, CcmLLRIntLongDoubleCanBeEvaluatedAtCompileTime)
{
	constexpr auto rint{ccm::llrint(1.0l)};
	static_assert(rint == 1.0l);
}

TEST(CcmathNearestTests, CcmLLRIntLLongDoubleCanBeEvaluatedAtCompileTime)
{
	constexpr auto rint{ccm::llrintl(1.0l)};
	static_assert(rint == 1.0l);
}
#endif

// Remove #define CLANG_LINUX
#ifdef CLANG_LINUX
	#undef CLANG_LINUX
#endif
