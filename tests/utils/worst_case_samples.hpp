/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace ccm::test::worst_case
{
	template <typename T>
	struct PowCase
	{
		T base;
		T exponent;
		const char * provenance;
	};

	struct PowBitPatternCase
	{
		std::uint64_t base_bits;
		std::uint64_t exponent_bits;
		const char * provenance;
	};

	// LLVM-libc sinf exceptional values (reviews.llvm.org/D123154).
	inline constexpr std::array<float, 6> kSinFloatExceptional = {
		0x1.4f0654p0F,
		-0x1.4f0654p0F,
		0x1.33333p13F,
		0x1.fbd9c8p22F,
		0x1.465d2p25F,
		0x1.47d0fep+34F,
	};

	inline constexpr std::array<float, 4> kCosFloatExceptional = {
		0x1.4f0654p0F,
		0x1.33333p13F,
		0x1.fbd9c8p22F,
		0x1.465d2p25F,
	};

	// Large-magnitude reduction stress (LLVM-libc sinf tables).
	inline constexpr std::array<float, 3> kSinFloatLargeReduction = {
		0x1.728fecp37F,
		0x1.628d4cp40F,
		-0x1.728fecp37F,
	};

	inline constexpr std::array<double, 4> kSinDoubleHard = {
		0x1.921fb54442d18p+1,
		0x1.2d97c7f3321d2p+0,
		0x1.45f306dc9c882p+2,
		-0x1.45f306dc9c882p+2,
	};

	// exp / expm1: tiny, subnormal-adjacent, overflow-adjacent, and split boundaries.
	inline constexpr std::array<float, 6> kExpm1FloatHard = {
		0x1.0p-25F,
		0x1.0p-24F,
		0x1.0p-2F,
		-0x1.0p-2F,
		0x1.62e42ep6F,
		-0x1.9fe368p6F,
	};

	inline constexpr std::array<double, 7> kExpm1DoubleHard = {
		0x1.0p-54,
		0x1.0p-53,
		0x1.0p-2,
		-0x1.0p-2,
		0x1.0p-10,
		0x1.62e42ep+6,
		-0x1.62e42ep+6,
	};

	inline constexpr std::array<float, 5> kExpFloatHard = {
		0x1.0p-25F,
		0x1.0p+0F,
		0x1.62e42ep6F,
		-0x1.0p+1F,
		-0x1.9fe368p6F,
	};

	inline constexpr std::array<double, 5> kExpDoubleHard = {
		0x1.0p-54,
		0x1.0p-1022,
		0x1.0p+0,
		0x1.62e42ep+6,
		-0x1.62e42ep+6,
	};

	// log / log2 / log10: hard-to-round near 1 and subnormal (Zimmermann TMD / glibc accuracy tables).
	inline constexpr std::array<double, 5> kLogDoubleHard = {
		0x1.0000000000001p+0,
		0x1.fffffffffffffp+0,
		0x1.62a88613629b6p+0,
		0x1.0p-1022,
		0x1.0p+1023,
	};

	inline constexpr std::array<float, 4> kLogFloatHard = {
		0x1.000002p+0F,
		0x1.fffffep+0F,
		0x1.0p-126F,
		0x1.0p+127F,
	};

	// log1p: inputs where 1+x loses bits and musl split boundaries.
	inline constexpr std::array<float, 6> kLog1pFloatHard = {
		0x1.0p-25F,
		0x1.0p-24F,
		0x1.6a09e667f3bcdp-1F,
		-0x1.6a09e667f3bcdp-1F,
		-0x1.0p-1F,
		0x1.0p+1F,
	};

	inline constexpr std::array<double, 6> kLog1pDoubleHard = {
		0x1.0p-53,
		0x1.0p-52,
		0x1.6a09e667f3bcdp-1,
		-0x1.6a09e667f3bcdp-1,
		-0x1.0p-1,
		0x1.0p+4,
	};

	// exp2 hard-to-round (Zimmermann, binary64 exp2 nearest).
	inline constexpr std::array<double, 3> kExp2DoubleHard = {
		0x1.e4596526bf94dp-10,
		-0x1.e4596526bf94dp-10,
		0x1.0p+10,
	};

	inline constexpr std::array<float, 3> kExp2FloatHard = {
		0x1.e4596526bf94dp-10F,
		-0x1.e4596526bf94dp-10F,
		0x1.0p+10F,
	};

	inline constexpr std::array<double, 4> kSqrtDoubleHard = {
		0x1.0p-1022,
		0x1.fffffffffffffp+1023,
		0x1.7ffffffffffffp+0,
		0x1.8000000000001p+0,
	};

	inline constexpr std::array<float, 4> kSqrtFloatHard = {
		0x1.0p-126F,
		0x1.fffffep+127F,
		0x1.7ffffep+0F,
		0x1.800002p+0F,
	};

	// Non-builtin path-matrix underflow residual campaign, seed 0xC0DEC0FFEE.
	inline constexpr std::array<PowBitPatternCase, 29> kPowDoubleUnderflowResidual = { {
		{ 0x251d5d43f50d0108ULL, 0x729c2b90e3442eecULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x4da248bc6a915c1eULL, 0xca93fd094b629db9ULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x67add96f434533e9ULL, 0xd16be30ac7782b1aULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x4180947677866edfULL, 0xea29cdd38c9dafe5ULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x2d6e4b3263a700ddULL, 0x66e5e2b8b62dae73ULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x065feef2d691057dULL, 0x47261da68f57948eULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x5971deedd54a73feULL, 0xcab0e9312c710c18ULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x00094b5d5317c3e7ULL, 0x7dbb01075a511900ULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x3c77173d214943f7ULL, 0x67ebc39f38f67d55ULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x0699eec8834512a3ULL, 0x6e78eef508a9755bULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x47c517385e76859bULL, 0xe5bf5928decba611ULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x3d57779a52896ccdULL, 0x44f68ab37e9830bfULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x7890d582c1f40d45ULL, 0xcacc034085c17047ULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x5dd7f4c26c0f13e7ULL, 0xf962e2383aff1c74ULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x4bbae25fcc4bc621ULL, 0xccf980b7d3eae02dULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x103f8a410b76302dULL, 0x45602c8f1fc72c13ULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x710c9c44942091b3ULL, 0xef1e832952550adeULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x3cb43cb6ffca5ea2ULL, 0x5f1869450668b0b9ULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x5f07fbb85d4e5ce4ULL, 0xd6c16d495050197dULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x788b201b3728b3a8ULL, 0xfb15dbf2644b141dULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x3ec66615e45a70d0ULL, 0x51172a84d617a0daULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x40012e1f44115d27ULL, 0xd16d1b9e135d9410ULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x1a419635071c475dULL, 0x49e26e56b9615aeeULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x07aaea00b075a713ULL, 0x5d9713d520484d14ULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x6797251914607e72ULL, 0xe3ffce54797ed86aULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x70d0fc8216be69f1ULL, 0xfafd1b08bd0ea065ULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x1cd3cfb993f9c5feULL, 0x5b355f216cca5f15ULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x5b44c6a8ee131f1cULL, 0xc72897d5346718c8ULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
		{ 0x20c2356781a19392ULL, 0x6ee4a8fe064ac7adULL, "non-builtin path-matrix underflow residual case, seed 0xC0DEC0FFEE" },
	} };

	inline constexpr std::array<PowCase<double>, 5> kPowDoubleHard = { {
		{ 0x1.0p-50, 0x1.0p+50, "legacy hard-case corpus: tiny base with huge positive exponent" },
		{ -0x1.0p+0, 0x1.8p+0, "legacy hard-case corpus: negative base near non-integer exponent domain edge" },
		{ 10.0, 4.0, "pow_gen regression case retained from PowGenDoubleRegressionCases" },
		{ 30.637028068178267, -7.702539522452998, "pow_gen regression case retained from PowGenDoubleRegressionCases" },
		{ 945971881662.053466796875, 15.38309228199631562, "pow_gen regression case retained from PowGenDoubleRegressionCases" },
	} };

	inline constexpr std::array<PowCase<float>, 4> kPowFloatHard = { {
		{ 0x1.0p-20F, 0x1.0p+20F, "float analogue of the legacy tiny-base / huge-exponent stress case" },
		{ -1.0F, 3.0F, "integer-parity sign propagation regression anchor" },
		{ -1.0F, 4.0F, "integer-parity sign propagation regression anchor" },
		{ -1.0F, 1.5F, "negative-base / non-integer exponent domain regression anchor" },
	} };

	// asin/acos: hard-to-round near ±1 and tiny (LLVM-libc / musl accuracy tables).
	inline constexpr std::array<double, 6> kAsinDoubleHard = {
		0x1.0p-1022,
		-0x1.0p-1022,
		0x1.fffffffffffffp-1,
		-0x1.fffffffffffffp-1,
		0x1.0p-27,
		-0x1.0p-27,
	};

	inline constexpr std::array<float, 5> kAsinFloatHard = {
		0x1.0p-126F,
		-0x1.0p-126F,
		0x1.fffffep-1F,
		-0x1.fffffep-1F,
		0x1.0p-14F,
	};

	inline constexpr std::array<double, 5> kAtanDoubleHard = {
		0x1.0p-1022,
		-0x1.0p-1022,
		0x1.0p+300,
		-0x1.0p+300,
		0x1.1p+0,
	};

	// cbrt: subnormal, overflow-adjacent, and hard-to-round cube-root neighbors.
	inline constexpr std::array<double, 6> kCbrtDoubleHard = {
		0x1.0p-1022,
		-0x1.0p-1022,
		0x1.fffffffffffffp+1023,
		-0x1.fffffffffffffp+1023,
		0x1.428a2f98d728bp+0,
		-0x1.428a2f98d728bp+0,
	};

	inline constexpr std::array<float, 5> kCbrtFloatHard = {
		0x1.0p-126F,
		-0x1.0p-126F,
		0x1.fffffep+127F,
		-0x1.fffffep+127F,
		0x1.428a2fp+0F,
	};

	// tgamma/lgamma: poles and large-magnitude stress (LLVM-libc tgamma tests).
	inline constexpr std::array<double, 8> kGammaDoubleHard = {
		0x1.0p-27,
		0x1.fffffffffffp-1,
		0x1.0p+4,
		0x1.0p+20,
		-0x1.0p-3,
		-0x1.8p+0,
		-0x1.0p+1,
		-0x1.0p+4,
	};

} // namespace ccm::test::worst_case
