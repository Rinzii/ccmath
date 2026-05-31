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

namespace ccm::test::samples
{
	inline constexpr std::array<float, 22> kTrigFloat = {
		0.0F, -0.0F, 0.001F, -0.001F, 0.1F, -0.1F, 0.25F, -0.25F, 0.5F, -0.5F, 1.0F, -1.0F, 1.5F, -1.5F, 2.0F, -2.0F,
		3.14159265F / 6.0F, 3.14159265F / 4.0F, 3.14159265F / 3.0F, 10.0F, -10.0F, 100.0F,
	};

	inline constexpr std::array<double, 21> kTrigDouble = {
		0.0, -0.0, 0.001, -0.001, 0.1, -0.1, 0.25, -0.25, 0.5, -0.5, 1.0, -1.0, 1.5, -1.5, 2.0, -2.0, 3.141592653589793 / 6.0,
		3.141592653589793 / 4.0, 3.141592653589793 / 3.0, 10.0, -10.0,
	};

	inline constexpr std::array<float, 14> kExpoFloat = {
		0.001F, 0.01F, 0.1F, 0.25F, 0.5F, 0.75F, 1.0F, 1.25F, 1.5F, 2.0F, 3.0F, 4.0F, 8.0F, 10.0F,
	};

	inline constexpr std::array<double, 18> kExpoDouble = {
		0.001, 0.01, 0.1, 0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 2.0, 3.0, 4.0, 8.0, 10.0, 16.0, 32.0, 64.0, 128.0,
	};

	inline constexpr std::array<float, 12> kSqrtFloat = { 0.0F, 0.01F, 0.1F, 0.25F, 0.5F, 1.0F, 2.0F, 3.0F, 4.0F, 8.0F, 16.0F, 1024.0F };

	inline constexpr std::array<double, 15> kSqrtDouble = { 0.0, 0.01, 0.1, 0.25, 0.5, 1.0, 2.0, 3.0, 4.0, 8.0, 16.0, 64.0, 128.0, 256.0, 1024.0 };

	inline constexpr std::array<double, 8> kPowBases = { 0.25, 0.5, 1.0, 2.0, 3.0, 4.0, 10.0, 14.0 };
	inline constexpr std::array<double, 9> kPowExponents = { -2.0, -1.0, -0.5, 0.0, 0.5, 1.0, 2.0, 3.0, 4.0 };

	inline constexpr std::array<float, 6> kPowFloatPairsBase = { 2.0F, 3.0F, 0.5F, 10.0F, 0.25F, 4.0F };
	inline constexpr std::array<float, 6> kPowFloatPairsExp = { 0.0F, 1.0F, 2.0F, -1.0F, 0.5F, 3.0F };

	// Small grids for all-rounding-mode ULP sweeps (keep test time bounded).
	inline constexpr std::array<double, 6> kAllModesProbeDouble = { 0.5, 1.0, 1.25, 2.0, 3.0, 10.0 };
	inline constexpr std::array<float, 5> kAllModesProbeFloat = { 0.5F, 1.0F, 1.5F, 2.0F, 4.0F };

	// Inputs where IEEE 754 / C++ results can depend on rounding mode (signed zero, ties).
	inline constexpr std::array<double, 4> kRoundingSensitiveDouble = { 0.0, -0.0, 1.0, -1.0 };
	inline constexpr std::array<float, 4> kRoundingSensitiveFloat = { 0.0F, -0.0F, 1.0F, -1.0F };

	inline constexpr std::array<double, 8> kNearbyIntProbeDouble = { 0.5, 1.5, 2.5, -0.5, -1.5, 123.4, -123.6, 1.0e10 + 0.5 };
	inline constexpr std::array<float, 6> kNearbyIntProbeFloat = { 0.5F, 1.5F, -0.5F, -1.5F, 123.4F, -123.6F };

	// Halfway and tie cases for round/rint (C++ rounds halfway away from zero for round).
	inline constexpr std::array<double, 10> kNearestHalfwayDouble = {
		0.5, 1.5, 2.5, -0.5, -1.5, -2.5, 0.25, -0.25, 123.5, -123.5,
	};
	inline constexpr std::array<float, 8> kNearestHalfwayFloat = { 0.5F, 1.5F, 2.5F, -0.5F, -1.5F, -2.5F, 123.5F, -123.5F };

	inline constexpr std::array<double, 4> kAllModesPowBases = { 2.0, 3.0, 0.5, 4.0 };
	inline constexpr std::array<double, 4> kAllModesPowExponents = { 2.0, 1.0, 2.0, 0.5 };

	// Inverse trig: asin/acos domain is [-1, 1]. atan accepts full range.
	inline constexpr std::array<float, 16> kInvTrigUnitFloat = {
		-1.0F, -0.99F, -0.75F, -0.5F, -0.25F, -0.125F, -0.0F, 0.0F,
		0.125F, 0.25F, 0.5F, 0.75F, 0.99F, 1.0F, 0x1.fffffep-1F, -0x1.fffffep-1F,
	};

	inline constexpr std::array<double, 16> kInvTrigUnitDouble = {
		-1.0, -0.99, -0.75, -0.5, -0.25, -0.125, -0.0, 0.0,
		0.125, 0.25, 0.5, 0.75, 0.99, 1.0, 0x1.fffffffffffffp-1, -0x1.fffffffffffffp-1,
	};

	inline constexpr std::array<float, 12> kAtanFloat = {
		-10.0F, -1.0F, -0.5F, -0.0F, 0.0F, 0.5F, 1.0F, 10.0F, 100.0F, -100.0F, 0x1.0p+20F, -0x1.0p+20F,
	};

	inline constexpr std::array<double, 12> kAtanDouble = {
		-10.0, -1.0, -0.5, -0.0, 0.0, 0.5, 1.0, 10.0, 100.0, -100.0, 0x1.0p+300, -0x1.0p+300,
	};

	// atan2 quadrant probes (y, x).
	inline constexpr std::array<float, 8> kAtan2YFloat = { 1.0F, 1.0F, -1.0F, -1.0F, 0.0F, 0.0F, 1.0F, -1.0F };
	inline constexpr std::array<float, 8> kAtan2XFloat = { 1.0F, -1.0F, 1.0F, -1.0F, 1.0F, -1.0F, 0.0F, 0.0F };

	inline constexpr std::array<double, 8> kAtan2YDouble = { 1.0, 1.0, -1.0, -1.0, 0.0, 0.0, 1.0, -1.0 };
	inline constexpr std::array<double, 8> kAtan2XDouble = { 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 0.0, 0.0 };

	inline constexpr std::array<float, 14> kCbrtFloat = {
		-27.0F, -8.0F, -1.0F, -0.0F, 0.0F, 0.125F, 1.0F, 8.0F, 27.0F, 64.0F, 1024.0F,
		0x1.0p-126F, 0x1.0p+30F, -0x1.0p+30F,
	};

	inline constexpr std::array<double, 17> kCbrtDouble = {
		-27.0, -8.0, -1.0, -0.0, 0.0, 0.125, 1.0, 8.0, 27.0, 64.0, 1024.0,
		0x1.0p-1022, 0x1.0p+300, -0x1.0p+300, 0x1.fffffffffffffp+1023, -0x1.fffffffffffffp+1023,
		0x1.428a2f98d728bp+0,
	};

	inline constexpr std::array<float, 6> kHypotPairXFloat = { 3.0F, 1e20F, 0.0F, 1.0F, 3.0F, 1e-30F };
	inline constexpr std::array<float, 6> kHypotPairYFloat = { 4.0F, 1e20F, 0.0F, 0.0F, -4.0F, 1e-30F };

	inline constexpr std::array<double, 6> kHypotPairXDouble = { 3.0, 1e200, 0.0, 1.0, 3.0, 1e-300 };
	inline constexpr std::array<double, 6> kHypotPairYDouble = { 4.0, 1e200, 0.0, 0.0, -4.0, 1e-300 };

	inline constexpr std::array<double, 8> kIlogbProbeDouble = {
		1.0, 0.125, 0.0, -0.0, 0x1.0p-1022, -0x1.0p-1022, 0x1.0p+1023, -0x1.0p+1023,
	};

	inline constexpr std::array<float, 6> kIlogbProbeFloat = {
		1.0F, 0.125F, 0.0F, -0.0F, 0x1.0p-126F, 0x1.0p+30F,
	};

	inline constexpr std::array<double, 8> kLogbProbeDouble = {
		8.0, 0.125, 0.0, -0.0, 0x1.0p-1022, 0x1.0p+1023, -0x1.0p+1023, 1.0,
	};

	inline constexpr std::array<double, 24> kGammaProbeDouble = {
		0.125, 0.5, 0.875, 1.5, 2.5, 3.75, 4.5, 7.25, 10.0, 12.5, 20.0, 30.0, 50.0, 100.0,
		-0.25, -1.5, -2.5, -3.25, -10.5, -15.25, 0x1.0p-20, 0x1.0p+4, 0x1.0p+10, 0x1.0p+20,
	};

	inline constexpr std::array<float, 16> kGammaProbeFloat = {
		0.125F, 0.5F, 1.5F, 2.5F, 3.75F, 7.25F, 10.0F, 20.0F, 50.0F,
		-0.25F, -1.5F, -2.5F, -10.5F, 0x1.0p-10F, 0x1.0p+4F, 0x1.0p+10F,
	};

} // namespace ccm::test::samples
