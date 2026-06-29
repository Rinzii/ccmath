/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// Correctly-rounded regression coverage for the binary64 pow kernel. Each case pins the
// exact result bit pattern produced by MPFR (correctly rounded, with subnormalize) under all
// four IEEE rounding modes. The cases exercise the kernel paths that previously rounded
// faithfully rather than correctly:
//   - subnormal results reached through the 2^-512 scale (double rounding into the subnormal
//     grid),
//   - over/underflow-adjacent results whose far-from-one base needs the e_x + log2(r) residual
//     carried into the accurate double-double,
//   - large integer-exponent results whose double-double exponentiation-by-squaring underflowed
//     its low limb, and
//   - negative-base integer exponents whose sign must be folded in before the directed rounding.

#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"

#include <gtest/gtest.h>

#include <array>
#include <cfenv>
#include <cstdint>
#include <cstring>

namespace
{
	double from_bits(std::uint64_t u)
	{
		double f{};
		std::memcpy(&f, &u, sizeof(f));
		return f;
	}

	std::uint64_t to_bits(double f)
	{
		std::uint64_t u{};
		std::memcpy(&u, &f, sizeof(u));
		return u;
	}

	struct CrCase
	{
		std::uint64_t base_bits;
		std::uint64_t exp_bits;
		std::array<std::uint64_t, 4> expected; // FE_TONEAREST, FE_TOWARDZERO, FE_UPWARD, FE_DOWNWARD
	};

	// Goldens generated with MPFR mpfr_pow at 53-bit target precision plus mpfr_subnormalize.
	constexpr CrCase kCases[] = {
		{ 0x405162815b601161ULL, 0xc064e4c1f2807ec5ULL, { 0x000885d3ea823897ULL, 0x000885d3ea823897ULL, 0x000885d3ea823898ULL, 0x000885d3ea823897ULL } },
		{ 0x3f2e4ddb428b4ea2ULL, 0x4055200000000000ULL, { 0x00248fc4d15a992fULL, 0x00248fc4d15a992fULL, 0x00248fc4d15a9930ULL, 0x00248fc4d15a992fULL } },
		{ 0x3ed92a0fe61fea87ULL, 0xc04d800000000000ULL, { 0x7fe5ee51f641fc88ULL, 0x7fe5ee51f641fc88ULL, 0x7fe5ee51f641fc89ULL, 0x7fe5ee51f641fc88ULL } },
		{ 0x3fa7ce5b56028880ULL, 0xc06ce00000000000ULL, { 0x7fd7dfa51b860ab3ULL, 0x7fd7dfa51b860ab3ULL, 0x7fd7dfa51b860ab4ULL, 0x7fd7dfa51b860ab3ULL } },
		{ 0xbff8652519a8b58aULL, 0xc06de00000000000ULL, { 0xb6d7a6dd226e1be5ULL, 0xb6d7a6dd226e1be4ULL, 0xb6d7a6dd226e1be4ULL, 0xb6d7a6dd226e1be5ULL } },
		{ 0xbffc8a7c01dee76eULL, 0x4074700000000000ULL, { 0xd1005d3d0c963798ULL, 0xd1005d3d0c963798ULL, 0xd1005d3d0c963798ULL, 0xd1005d3d0c963799ULL } },
		{ 0x4173b2e892108fcaULL, 0x4037000000000000ULL, { 0x62dde05268d268ebULL, 0x62dde05268d268ebULL, 0x62dde05268d268ecULL, 0x62dde05268d268ebULL } },
		// nextafter(2^k) bases raised to integer powers: results sit just below a power of two with
		// a sub-ulp tail the double-double cannot place, so the exponent-tracked ipow must resolve
		// the directed rounding. Tiny power, reciprocal of a tiny power, and a near-overflow power.
		{ 0x3c4fffffffffffffULL, 0x4031000000000000ULL, { 0x024fffffffffffefULL, 0x024fffffffffffefULL, 0x024ffffffffffff0ULL, 0x024fffffffffffefULL } },
		{ 0x3ca0000000000001ULL, 0xc033000000000000ULL, { 0x7edfffffffffffdaULL, 0x7edfffffffffffdaULL, 0x7edfffffffffffdbULL, 0x7edfffffffffffdaULL } },
		{ 0x429fffffffffffffULL, 0x4037000000000000ULL, { 0x7dbfffffffffffe9ULL, 0x7dbfffffffffffe9ULL, 0x7dbfffffffffffeaULL, 0x7dbfffffffffffe9ULL } },
		// Negative base with integer exponent overflowing: directed rounding must keep the sign
		// (positive even-power overflow vs negative odd-power overflow).
		{ 0xc4b2b5154aedb961ULL, 0x406dc00000000000ULL, { 0x7ff0000000000000ULL, 0x7fefffffffffffffULL, 0x7ff0000000000000ULL, 0x7fefffffffffffffULL } },
		{ 0xc2886f7925646c38ULL, 0x4044800000000000ULL, { 0xfff0000000000000ULL, 0xffefffffffffffffULL, 0xffefffffffffffffULL, 0xfff0000000000000ULL } },
		// x^(-1/2) = 1/sqrt(x) just past a power of two: a double-double 1/sqrt is a directed-mode
		// ULP short, so an exact residual test pins the rounding (cr_rsqrt).
		{ 0x3370000000000001ULL, 0xbfe0000000000000ULL, { 0x462fffffffffffffULL, 0x462fffffffffffffULL, 0x4630000000000000ULL, 0x462fffffffffffffULL } },
		{ 0x336fffffffffffffULL, 0xbfe0000000000000ULL, { 0x4630000000000000ULL, 0x4630000000000000ULL, 0x4630000000000001ULL, 0x4630000000000000ULL } },
		{ 0x3ff0000000000001ULL, 0xbfe0000000000000ULL, { 0x3fefffffffffffffULL, 0x3fefffffffffffffULL, 0x3ff0000000000000ULL, 0x3fefffffffffffffULL } },
		// Integer power whose result is subnormal: the accurate squaring rounds straight onto the
		// subnormal grid rather than double-rounding through the kernel.
		{ 0x3b5fffffffffffffULL, 0x402c000000000000ULL, { 0x000ffffffffffff9ULL, 0x000ffffffffffff9ULL, 0x000ffffffffffffaULL, 0x000ffffffffffff9ULL } },
	};

	constexpr std::array<int, 4> kModes			 = { FE_TONEAREST, FE_TOWARDZERO, FE_UPWARD, FE_DOWNWARD };
	constexpr std::array<const char *, 4> kNames = { "FE_TONEAREST", "FE_TOWARDZERO", "FE_UPWARD", "FE_DOWNWARD" };
} // namespace

TEST(CcmathPowCorrectRounding, BinaryDoubleAllModes)
{
	const int saved = std::fegetround();
	for (const auto & c : kCases)
	{
		const double base = from_bits(c.base_bits);
		const double exp  = from_bits(c.exp_bits);
		for (std::size_t m = 0; m < kModes.size(); ++m)
		{
			std::fesetround(kModes[m]);
			const double got = ccm::gen::pow_gen<double>(base, exp);
			std::fesetround(saved);

			SCOPED_TRACE(kNames[m]);
			SCOPED_TRACE(c.base_bits);
			SCOPED_TRACE(c.exp_bits);
			EXPECT_EQ(to_bits(got), c.expected[m]);
		}
	}
	std::fesetround(saved);
}
