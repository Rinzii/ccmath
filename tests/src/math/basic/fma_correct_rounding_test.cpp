/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// Correctly-rounded gate for the software fma. fma is defined to round the exact x*y + z once,
// so the ccmath fixed-accumulator kernel must reproduce the exact result bit pattern under every
// IEEE rounding mode. Each case pins the value MPFR produces (correctly rounded, with
// subnormalize). These goldens are used instead of std::fma because an optimized build does not
// honour the dynamic rounding mode for the libm call (FENV_ACCESS is not portably available), so
// std::fma is not a trustworthy directed-mode oracle for overflow and inexact-normal results.
// The cases cover the classic double-rounding killers, an inexact normal result, exact results,
// and the overflow and underflow boundaries where directed rounding diverges from round-to-nearest.

#include "ccmath/ccmath.hpp"

#include <gtest/gtest.h>

#include <cfenv>
#include <cstdint>
#include <cstring>

namespace
{
	float f_from_bits(std::uint32_t u)
	{
		float f{};
		std::memcpy(&f, &u, sizeof(f));
		return f;
	}
	std::uint32_t f_to_bits(float f)
	{
		std::uint32_t u{};
		std::memcpy(&u, &f, sizeof(u));
		return u;
	}
	double d_from_bits(std::uint64_t u)
	{
		double f{};
		std::memcpy(&f, &u, sizeof(f));
		return f;
	}
	std::uint64_t d_to_bits(double f)
	{
		std::uint64_t u{};
		std::memcpy(&u, &f, sizeof(u));
		return u;
	}

	struct FloatCase
	{
		std::uint32_t x, y, z;
		std::uint32_t expected[4]; // FE_TONEAREST, FE_TOWARDZERO, FE_UPWARD, FE_DOWNWARD
	};
	struct DoubleCase
	{
		std::uint64_t x, y, z;
		std::uint64_t expected[4];
	};

	// Goldens generated with MPFR mpfr_fma at the target precision plus mpfr_subnormalize.
	constexpr FloatCase kFloatCases[] = {
		{ 0x3f800800U, 0x3f800800U, 0x25000000U, { 0x3f801001U, 0x3f801000U, 0x3f801001U, 0x3f801000U } },
		{ 0x3f800800U, 0x3f800800U, 0xa5000000U, { 0x3f801000U, 0x3f801000U, 0x3f801001U, 0x3f801000U } },
		{ 0x3f7ff800U, 0x3f7ff800U, 0x24800000U, { 0x3f7ff000U, 0x3f7ff000U, 0x3f7ff001U, 0x3f7ff000U } },
		{ 0x3f801000U, 0x3f800400U, 0x25800000U, { 0x3f801401U, 0x3f801400U, 0x3f801401U, 0x3f801400U } },
		{ 0xc27a0cd9U, 0x5302830dU, 0x96599102U, { 0xd5fef50fU, 0xd5fef50eU, 0xd5fef50eU, 0xd5fef50fU } },
		{ 0x40400000U, 0x40a00000U, 0x40e00000U, { 0x41b00000U, 0x41b00000U, 0x41b00000U, 0x41b00000U } },
		{ 0x71aba7f8U, 0x58635fa9U, 0x3f800000U, { 0x7f800000U, 0x7f7fffffU, 0x7f800000U, 0x7f7fffffU } },
		{ 0xf1aba7f8U, 0x58635fa9U, 0x3f800000U, { 0xff800000U, 0xff7fffffU, 0xff7fffffU, 0xff800000U } },
		{ 0x0da24260U, 0x26901d7dU, 0x00000000U, { 0x00000001U, 0x00000000U, 0x00000001U, 0x00000000U } },
	};

	constexpr DoubleCase kDoubleCases[] = {
		{ 0x3ff0000002000000ULL,
		  0x3ff0000002000000ULL,
		  0x3950000000000000ULL,
		  { 0x3ff0000004000000ULL, 0x3ff0000004000000ULL, 0x3ff0000004000001ULL, 0x3ff0000004000000ULL } },
		{ 0x3ff0000002000000ULL,
		  0x3ff0000002000000ULL,
		  0xb950000000000000ULL,
		  { 0x3ff0000004000000ULL, 0x3ff0000004000000ULL, 0x3ff0000004000001ULL, 0x3ff0000004000000ULL } },
		{ 0xc04f419b20000000ULL,
		  0x42605061a0000000ULL,
		  0xbacb322040000000ULL,
		  { 0xc2bfdea1ddec8140ULL, 0xc2bfdea1ddec8140ULL, 0xc2bfdea1ddec8140ULL, 0xc2bfdea1ddec8141ULL } },
		{ 0x4008000000000000ULL,
		  0x4014000000000000ULL,
		  0x401c000000000000ULL,
		  { 0x4036000000000000ULL, 0x4036000000000000ULL, 0x4036000000000000ULL, 0x4036000000000000ULL } },
		{ 0x7e444ecd0d33972bULL,
		  0x46293e5939a08ceaULL,
		  0x3ff0000000000000ULL,
		  { 0x7ff0000000000000ULL, 0x7fefffffffffffffULL, 0x7ff0000000000000ULL, 0x7fefffffffffffffULL } },
		{ 0xfe444ecd0d33972bULL,
		  0x46293e5939a08ceaULL,
		  0x3ff0000000000000ULL,
		  { 0xfff0000000000000ULL, 0xffefffffffffffffULL, 0xffefffffffffffffULL, 0xfff0000000000000ULL } },
		{ 0x01a56e1fc2f8f359ULL,
		  0x39b4484bfeebc2a0ULL,
		  0x0000000000000000ULL,
		  { 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000001ULL, 0x0000000000000000ULL } },
		{ 0x3fffffffffffffffULL,
		  0x3ff0000000000001ULL,
		  0x3c30000000000000ULL,
		  { 0x4000000000000001ULL, 0x4000000000000000ULL, 0x4000000000000001ULL, 0x4000000000000000ULL } },
	};

	constexpr int kModes[4]		   = { FE_TONEAREST, FE_TOWARDZERO, FE_UPWARD, FE_DOWNWARD };
	constexpr const char* kName[4] = { "FE_TONEAREST", "FE_TOWARDZERO", "FE_UPWARD", "FE_DOWNWARD" };
} // namespace

TEST(CcmathFmaCorrectRounding, FloatAllModes)
{
	const int saved = std::fegetround();
	for (const auto& c : kFloatCases)
	{
		const float x = f_from_bits(c.x);
		const float y = f_from_bits(c.y);
		const float z = f_from_bits(c.z);
		for (int m = 0; m < 4; ++m)
		{
			std::fesetround(kModes[m]);
			const float got = ccm::fma(x, y, z);
			std::fesetround(saved);
			SCOPED_TRACE(kName[m]);
			EXPECT_EQ(f_to_bits(got), c.expected[m]) << "x=0x" << std::hex << c.x << " y=0x" << c.y << " z=0x" << c.z;
		}
	}
	std::fesetround(saved);
}

TEST(CcmathFmaCorrectRounding, DoubleAllModes)
{
	const int saved = std::fegetround();
	for (const auto& c : kDoubleCases)
	{
		const double x = d_from_bits(c.x);
		const double y = d_from_bits(c.y);
		const double z = d_from_bits(c.z);
		for (int m = 0; m < 4; ++m)
		{
			std::fesetround(kModes[m]);
			const double got = ccm::fma(x, y, z);
			std::fesetround(saved);
			SCOPED_TRACE(kName[m]);
			EXPECT_EQ(d_to_bits(got), c.expected[m]) << "x=0x" << std::hex << c.x << " y=0x" << c.y << " z=0x" << c.z;
		}
	}
	std::fesetround(saved);
}
