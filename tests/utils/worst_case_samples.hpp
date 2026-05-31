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

namespace ccm::test::worst_case
{
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

	inline constexpr std::array<double, 4> kPowDoubleHard = {
		0x1.0p-50,
		0x1.0p+50,
		-0x1.0p+0,
		0x1.8p+0,
	};

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
