/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// Regression for the MSVC extern "C" libm linkage collision (errors C2733 and C2375).
//
// The runtime kernels used to forward declare libm functions (exp2, log, log2, log10, sin, cos,
// tan, fmod, remainder, tgamma, lgamma and their float variants) as extern "C" inside a namespace.
// A function with C language linkage declared in a namespace names the same entity as the global
// one, so once a consumer pulled in the UCRT declarations from the system math headers alongside
// ccmath, MSVC rejected the two declarations as conflicting.
//
// The system math headers are included first on purpose, then ccmath, which is the order a downstream
// consumer hits. If the forward declarations ever come back this translation unit stops compiling on
// MSVC. The runtime checks confirm the kernels still resolve and return the expected values.

// cmath must be parsed before ccmath so the UCRT declarations are already in scope (cmath pulls in
// corecrt_math.h, the header named in the original error), which is what triggered the collision.
// Keep this order fixed and do not let the formatter sort it.
// clang-format off
#include <cmath>

#include <ccmath/ccmath.hpp>
// clang-format on

// Pull in the runtime headers that carried the forward declarations directly, so the guard holds even
// if the umbrella header stops including one of them.
#include "ccmath/internal/math/runtime/func/basic/fmod_rt.hpp"
#include "ccmath/internal/math/runtime/func/basic/remainder_rt.hpp"
#include "ccmath/internal/math/runtime/func/expo/exp2_rt.hpp"
#include "ccmath/internal/math/runtime/func/expo/log10_rt.hpp"
#include "ccmath/internal/math/runtime/func/expo/log2_rt.hpp"
#include "ccmath/internal/math/runtime/func/expo/log_rt.hpp"
#include "ccmath/internal/math/runtime/func/misc/gamma_rt.hpp"
#include "ccmath/internal/math/runtime/func/misc/lgamma_rt.hpp"
#include "ccmath/internal/math/runtime/func/trig/cos_rt.hpp"
#include "ccmath/internal/math/runtime/func/trig/sin_rt.hpp"
#include "ccmath/internal/math/runtime/func/trig/tan_rt.hpp"

#include <gtest/gtest.h>

namespace
{
	// Force a genuine runtime call so the runtime dispatch is exercised rather than constant folded.
	template <typename T>
	T runtime(T value)
	{
		volatile T sink = value;
		return sink;
	}
} // namespace

TEST(CcmathSystemMathHeaderCoexistence, RuntimeKernelsResolveAlongsideSystemMath)
{
	EXPECT_DOUBLE_EQ(ccm::exp2(runtime(3.0)), 8.0);
	EXPECT_DOUBLE_EQ(ccm::log(runtime(1.0)), 0.0);
	EXPECT_DOUBLE_EQ(ccm::log2(runtime(8.0)), 3.0);
	EXPECT_DOUBLE_EQ(ccm::log10(runtime(100.0)), 2.0);
	EXPECT_DOUBLE_EQ(ccm::sin(runtime(0.0)), 0.0);
	EXPECT_DOUBLE_EQ(ccm::cos(runtime(0.0)), 1.0);
	EXPECT_DOUBLE_EQ(ccm::tan(runtime(0.0)), 0.0);
	EXPECT_DOUBLE_EQ(ccm::fmod(runtime(7.0), runtime(3.0)), 1.0);
	EXPECT_DOUBLE_EQ(ccm::remainder(runtime(7.0), runtime(3.0)), 1.0);
	EXPECT_DOUBLE_EQ(ccm::tgamma(runtime(5.0)), 24.0);
	EXPECT_DOUBLE_EQ(ccm::lgamma(runtime(1.0)), 0.0);

	EXPECT_FLOAT_EQ(ccm::exp2f(runtime(3.0F)), 8.0F);
	EXPECT_FLOAT_EQ(ccm::tgammaf(runtime(5.0F)), 24.0F);
}

TEST(CcmathSystemMathHeaderCoexistence, RuntimeEntryPointsResolveAlongsideSystemMath)
{
	// The runtime entry points are what included the previously conflicting headers, so call them too.
	EXPECT_DOUBLE_EQ(ccm::rt::exp2_rt(runtime(3.0)), 8.0);
	EXPECT_DOUBLE_EQ(ccm::rt::sin_rt(runtime(0.0)), 0.0);
	EXPECT_DOUBLE_EQ(ccm::rt::gamma_rt(runtime(5.0)), 24.0);
	EXPECT_DOUBLE_EQ(ccm::rt::remainder_rt(runtime(7.0), runtime(3.0)), 1.0);
}
