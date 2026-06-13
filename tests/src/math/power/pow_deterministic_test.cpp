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
#include "ccmath/internal/support/bits.hpp"

#include <gtest/gtest.h>

#include <array>
#include <cstdint>

// These checks only carry meaning in a deterministic build (CCMATH_ENABLE_DETERMINISTIC=ON).
// In a default build the body compiles to a trivial pass so the file stays in the normal suite.
#ifdef CCM_CONFIG_DETERMINISTIC

namespace
{
	template <typename T>
	bool same_bits(T a, T b) noexcept
	{
		using U = std::conditional_t<sizeof(T) == 4, std::uint32_t, std::uint64_t>;
		return ccm::support::bit_cast<U>(a) == ccm::support::bit_cast<U>(b);
	}

	constexpr std::array<double, 6> kBases	   = { 2.0, 0.5, 1.25, 3.0, 10.0, 0.1 };
	constexpr std::array<double, 6> kExponents = { 3.0, -2.0, 0.5, 7.25, -0.75, 100.0 };
} // namespace

// Deterministic mode must route the public runtime entry point through the generic kernel rather
// than system libm. That makes ccm::pow bit-identical to gen::pow_gen on every host.
TEST(PowDeterministic, DoubleRuntimeMatchesGenericKernel)
{
	for (double base : kBases)
	{
		for (double exp : kExponents)
		{
			const double via_public	 = ccm::pow(base, exp);
			const double via_generic = ccm::gen::pow_gen(base, exp);
			EXPECT_TRUE(same_bits(via_public, via_generic)) << "base=" << base << " exp=" << exp;
		}
	}
}

TEST(PowDeterministic, FloatRuntimeMatchesGenericKernel)
{
	for (double base : kBases)
	{
		for (double exp : kExponents)
		{
			const auto b			= static_cast<float>(base);
			const auto e			= static_cast<float>(exp);
			const float via_public	= ccm::pow(b, e);
			const float via_generic = ccm::gen::pow_gen(b, e);
			EXPECT_TRUE(same_bits(via_public, via_generic)) << "base=" << b << " exp=" << e;
		}
	}
}

// Long double has no cross-hardware-portable format, so deterministic mode evaluates it in double
// precision: powl(x, y) must equal (long double)pow((double)x, (double)y).
TEST(PowDeterministic, LongDoubleDelegatesToDouble)
{
	for (double base : kBases)
	{
		for (double exp : kExponents)
		{
			const auto b			= static_cast<long double>(base);
			const auto e			= static_cast<long double>(exp);
			const long double got	= ccm::pow(b, e);
			const long double want	= static_cast<long double>(ccm::pow(static_cast<double>(b), static_cast<double>(e)));
			EXPECT_TRUE(same_bits(got, want)) << "base=" << base << " exp=" << exp;
		}
	}
}

// Compile-time evaluation also routes through the generic kernel, so a constexpr result is
// available (the point of a deterministic, libm-free build).
TEST(PowDeterministic, CompileTimeUsesGenericKernel)
{
	constexpr double ct = ccm::pow(2.0, 10.0);
	static_assert(ct == 1024.0, "deterministic pow must fold 2^10 at compile time");
	EXPECT_DOUBLE_EQ(ct, 1024.0);
}

#else

TEST(PowDeterministic, DisabledWithoutFlag)
{ GTEST_SKIP() << "CCMATH_ENABLE_DETERMINISTIC is OFF; deterministic guarantees are not in effect."; }

#endif // CCM_CONFIG_DETERMINISTIC
