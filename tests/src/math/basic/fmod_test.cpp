/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "utils/std_compare.hpp"
#include "utils/test_runtime.hpp"

#include <gtest/gtest.h>

#include <ccmath/ccmath.hpp>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <random>

using ccm::test::runtime_value;

namespace
{
	// fmod is an exact operation, so every result must match the host libm bit for bit, not within a
	// ULP band. ccm::fmod is the public entry (builtin or kernel); ccm::internal::fmod is the kernel
	// the constexpr and no-builtin paths actually run.
	void expect_public_exact(double x, double y)
	{
		SCOPED_TRACE(testing::Message() << "fmod(" << x << ", " << y << ")");
		ccm::test::ExpectFpEq(ccm::fmod(x, y), std::fmod(x, y));
	}
	void expect_public_exact_f(float x, float y)
	{
		SCOPED_TRACE(testing::Message() << "fmodf(" << x << ", " << y << ")");
		ccm::test::ExpectFpEq(ccm::fmod(x, y), std::fmod(x, y));
	}
	// Exercises the chunked-modulo kernel directly at runtime (the public path uses __builtin_fmod on
	// GCC/Clang, so without this the kernel only has compile-time coverage). runtime_value defeats
	// constant folding so the kernel really executes.
	void expect_kernel_exact(double x, double y)
	{
		SCOPED_TRACE(testing::Message() << "kernel fmod(" << x << ", " << y << ")");
		ccm::test::ExpectFpEq(ccm::internal::fmod(runtime_value(x), runtime_value(y)), std::fmod(x, y));
	}
	void expect_kernel_exact_f(float x, float y)
	{
		SCOPED_TRACE(testing::Message() << "kernel fmodf(" << x << ", " << y << ")");
		ccm::test::ExpectFpEq(ccm::internal::fmod(runtime_value(x), runtime_value(y)), std::fmod(x, y));
	}
} // namespace

TEST(CcmathBasicTests, FmodLargeQuotientCompileTime)
{
	// Large quotients must still reduce exactly at compile time, even when abs(x / y) is far above 2^53.
	static_assert(ccm::fmod(1e30, 3.0) == 1.0, "fmod(1e30, 3) must be 1");
	static_assert(ccm::fmod(-1e30, 3.0) == -1.0, "fmod(-1e30, 3) must be -1");
	static_assert(ccm::fmod(1e300, 7.0) == 1.0, "fmod(1e300, 7) must reduce exactly to 1");
	static_assert(ccm::fmod(1e30F, 3.0F) == 0.0F, "fmodf(1e30, 3) must reduce exactly to 0");

	// Small / normal cases stay exact.
	static_assert(ccm::fmod(10.0, 3.0) == 1.0, "fmod(10, 3) must be 1");
	static_assert(ccm::fmod(-10.0, 3.0) == -1.0, "fmod(-10, 3) must be -1");
	static_assert(ccm::fmod(7.5, 2.0) == 1.5, "fmod(7.5, 2) must be 1.5");
	static_assert(ccm::fmod(1, 2) == 1, "fmod must be usable in a static_assert");
}

TEST(CcmathBasicTests, FmodBitExactVsStd)
{
	for (double y : { 3.0, -3.0, 2.5, 0.5, 1024.0, 1e-300 })
	{
		for (double x : { 10.0, -10.0, 7.5, -7.5, 0.0, -0.0, 1.5, 1e30, -1e30, 1e300 })
		{
			expect_public_exact(x, y);
			expect_kernel_exact(x, y);
		}
	}
	for (float y : { 3.0F, -3.0F, 2.5F, 0.5F })
	{
		for (float x : { 10.0F, -10.0F, 7.5F, 0.0F, -0.0F, 1e30F })
		{
			expect_public_exact_f(x, y);
			expect_kernel_exact_f(x, y);
		}
	}
}

TEST(CcmathBasicTests, FmodKernelRandomBitExactVsStd)
{
	// Stress the runtime kernel against the host libm on random finite pairs, bit for bit.
	std::mt19937_64 rng(0xF110D);
	std::uniform_int_distribution<std::uint64_t> bd(0, ~std::uint64_t(0));
	int checked = 0;
	while (checked < 6000)
	{
		double x;
		double y;
		const std::uint64_t bx = bd(rng);
		const std::uint64_t by = bd(rng);
		std::memcpy(&x, &bx, sizeof x);
		std::memcpy(&y, &by, sizeof y);
		if (!std::isfinite(x) || !std::isfinite(y) || y == 0.0) { continue; }
		ccm::test::ExpectFpEq(ccm::internal::fmod(x, y), std::fmod(x, y));
		++checked;
	}

	std::mt19937 rngf(0xF110F);
	std::uniform_int_distribution<std::uint32_t> bf(0, ~std::uint32_t(0));
	checked = 0;
	while (checked < 6000)
	{
		float x;
		float y;
		const std::uint32_t bx = bf(rngf);
		const std::uint32_t by = bf(rngf);
		std::memcpy(&x, &bx, sizeof x);
		std::memcpy(&y, &by, sizeof y);
		if (!std::isfinite(x) || !std::isfinite(y) || y == 0.0F) { continue; }
		ccm::test::ExpectFpEq(ccm::internal::fmod(x, y), std::fmod(x, y));
		++checked;
	}
}

TEST(CcmathBasicTests, FmodSignedZeroResult)
{
	// A zero remainder (exact multiple, or equal magnitudes) keeps the sign of x.
	ccm::test::ExpectSignedZero(ccm::fmod(6.0, 3.0), false);
	ccm::test::ExpectSignedZero(ccm::fmod(-6.0, 3.0), true);
	ccm::test::ExpectSignedZero(ccm::fmod(3.0, 3.0), false);
	ccm::test::ExpectSignedZero(ccm::fmod(-3.0, 3.0), true);
	ccm::test::ExpectSignedZero(ccm::internal::fmod(runtime_value(6.0), runtime_value(3.0)), false);
	ccm::test::ExpectSignedZero(ccm::internal::fmod(runtime_value(-6.0), runtime_value(3.0)), true);
	ccm::test::ExpectSignedZero(ccm::fmod(6.0F, 3.0F), false);
	ccm::test::ExpectSignedZero(ccm::fmod(-6.0F, 3.0F), true);

	// fmod(+/-0, y) returns +/-0 for non-zero y.
	ccm::test::ExpectSignedZero(ccm::fmod(0.0, 3.0), false);
	ccm::test::ExpectSignedZero(ccm::fmod(-0.0, 3.0), true);
}

TEST(CcmathBasicTests, FmodSpecialCases)
{
	constexpr double inf = std::numeric_limits<double>::infinity();
	constexpr double nan = std::numeric_limits<double>::quiet_NaN();

	// fmod(x, +/-inf) == x for finite x.
	expect_public_exact(10.0, inf);
	expect_public_exact(10.0, -inf);
	expect_public_exact(-7.5, inf);

	// Domain errors yield NaN: fmod(x, 0), fmod(+/-inf, y), and NaN in either argument.
	EXPECT_TRUE(std::isnan(ccm::fmod(10.0, 0.0)));
	EXPECT_TRUE(std::isnan(ccm::fmod(inf, 3.0)));
	EXPECT_TRUE(std::isnan(ccm::fmod(-inf, 3.0)));
	EXPECT_TRUE(std::isnan(ccm::fmod(nan, 3.0)));
	EXPECT_TRUE(std::isnan(ccm::fmod(3.0, nan)));
	EXPECT_TRUE(std::isnan(ccm::internal::fmod(runtime_value(inf), runtime_value(3.0))));
	EXPECT_TRUE(std::isnan(ccm::internal::fmod(runtime_value(10.0), runtime_value(0.0))));
}

TEST(CcmathBasicTests, FmodSubnormal)
{
	constexpr double dmin = std::numeric_limits<double>::denorm_min();
	expect_public_exact(1.5, 2.781342323457793e-309); // normal dividend, subnormal divisor
	expect_kernel_exact(1.5, 2.781342323457793e-309);
	expect_kernel_exact(5.0 * dmin, 2.0 * dmin); // subnormal dividend and divisor
	expect_kernel_exact(7.0 * dmin, 3.0 * dmin);
	expect_kernel_exact(1e300, dmin); // largest gap (normal over min subnormal)
	expect_kernel_exact(std::numeric_limits<double>::max(), dmin);
}

TEST(CcmathBasicTests, FmodTypesAndOverloads)
{
	// Integer overload promotes to double.
	ccm::test::ExpectFpEq(ccm::fmod(10, 3), std::fmod(10, 3));
	static_assert(std::is_same_v<decltype(ccm::fmod(10, 3)), double>, "integer fmod must return double");

	// fmodf / fmodl wrappers. long double delegates to the double kernel by design, so use values that
	// are exact through double.
	ccm::test::ExpectFpEq(ccm::fmodf(10.0F, 3.0F), std::fmod(10.0F, 3.0F));
	EXPECT_EQ(ccm::fmodl(10.0L, 3.0L), 1.0L);
	EXPECT_EQ(ccm::fmodl(7.5L, 2.0L), 1.5L);
	EXPECT_EQ(ccm::fmodl(-10.0L, 3.0L), -1.0L);
}
