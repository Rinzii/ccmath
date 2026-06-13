/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// Regression coverage for the generic trig kernels (the path used for constexpr and the
// non-builtin runtime). Pins the defects fixed in the trig overhaul: the large-argument
// reduction collapse, the float-grade double kernel, the sign of a signed zero, the asin
// tiny-|x| cubic sign, and the float atan accuracy. Goldens are MPFR correctly-rounded values;
// the accuracy contract is 4 ULP.

#include "ccmath/internal/math/generic/func/trig/cos_gen.hpp"
#include "ccmath/internal/math/generic/func/trig/sin_gen.hpp"
#include "ccmath/internal/math/generic/func/trig/tan_gen.hpp"
#include "ccmath/math/trig/impl/inv_trig_impl.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>

namespace
{
	std::uint64_t d_bits(double v)
	{
		std::uint64_t u{};
		std::memcpy(&u, &v, sizeof(u));
		return u;
	}
	std::uint32_t f_bits(float v)
	{
		std::uint32_t u{};
		std::memcpy(&u, &v, sizeof(u));
		return u;
	}
	double d_from(std::uint64_t u)
	{
		double v{};
		std::memcpy(&v, &u, sizeof(v));
		return v;
	}
	float f_from(std::uint32_t u)
	{
		float v{};
		std::memcpy(&v, &u, sizeof(v));
		return v;
	}

	long long d_mono(double v)
	{
		std::uint64_t u = d_bits(v);
		return (u & 0x8000000000000000ULL) ? -static_cast<long long>(u & 0x7fffffffffffffffULL) : static_cast<long long>(u | 0x8000000000000000ULL);
	}
	long long f_mono(float v)
	{
		std::uint32_t u = f_bits(v);
		return (u & 0x80000000U) ? -static_cast<long long>(u & 0x7fffffffU) : static_cast<long long>(u | 0x80000000U);
	}
	long long d_ulp(double a, double b) { long long d = d_mono(a) - d_mono(b); return d < 0 ? -d : d; }
	long long f_ulp(float a, float b) { long long d = f_mono(a) - f_mono(b); return d < 0 ? -d : d; }
} // namespace

TEST(CcmathTrigKernelRegression, DoubleWithinContract)
{
	// {arg_bits, golden_bits} from MPFR. sin(1.5) was float-grade; the large args collapsed.
	EXPECT_LE(d_ulp(ccm::gen::sin_gen<double>(d_from(0x3ff8000000000000ULL)), d_from(0x3fefeb7a9b2c6d8bULL)), 4); // sin(1.5)
	EXPECT_LE(d_ulp(ccm::gen::sin_gen<double>(d_from(0x4170000000000000ULL)), d_from(0xbfe8f22f8433d6eeULL)), 4); // sin(2^24)
	EXPECT_LE(d_ulp(ccm::gen::sin_gen<double>(d_from(0x430c6bf526340000ULL)), d_from(0x3feb76f88136cebaULL)), 4); // sin(1e15)
	EXPECT_LE(d_ulp(ccm::gen::sin_gen<double>(d_from(0x7feffffc57ca82aeULL)), d_from(0x3febee37b4bf8bb2ULL)), 4); // sin(~1.8e308)
	EXPECT_LE(d_ulp(ccm::gen::cos_gen<double>(d_from(0x419d6f3454000000ULL)), d_from(0x3fc1f4077c91589fULL)), 4); // cos(123456789)
}

TEST(CcmathTrigKernelRegression, FloatWithinContract)
{
	EXPECT_LE(f_ulp(ccm::gen::sin_gen<float>(f_from(0x4b800000U)), f_from(0xbf47917cU)), 4); // sin(2^24)
	EXPECT_LE(f_ulp(ccm::gen::cos_gen<float>(f_from(0x60ad78ecU)), f_from(0x3f411723U)), 4); // cos(1e20)
	EXPECT_LE(f_ulp(ccm::gen::sin_gen<float>(f_from(0x494cccc6U)), f_from(0xbf7cc946U)), 4); // sin(838860.4)
	EXPECT_LE(f_ulp(ccm::internal::impl::atan_impl<float>(f_from(0x3f210625U)), f_from(0x3f0fbc89U)), 4); // atan(0.629)
}

TEST(CcmathTrigKernelRegression, AsinTinyArgument)
{
	// The tiny-|x| fast path used the wrong cubic sign (returned x - x^3/6 instead of x + x^3/6).
	EXPECT_LE(d_ulp(ccm::internal::impl::asin_impl<double>(d_from(0x3f0f75104d551d69ULL)), d_from(0x3f0f75104da62dffULL)), 4); // asin(6e-5)
	EXPECT_EQ(d_bits(ccm::internal::impl::asin_impl<double>(d_from(0x3ddb7cdfd9d7bdbbULL))), 0x3ddb7cdfd9d7bdbbULL);				// asin(1e-10) = x
	// asin(x) must exceed x for small x > 0.
	EXPECT_GT(ccm::internal::impl::asin_impl<double>(6e-5), 6e-5);
}

TEST(CcmathTrigKernelRegression, SignOfZero)
{
	EXPECT_EQ(d_bits(ccm::gen::sin_gen<double>(-0.0)), 0x8000000000000000ULL); // sin(-0) = -0
	EXPECT_EQ(d_bits(ccm::gen::sin_gen<double>(0.0)), 0x0ULL);				   // sin(+0) = +0
	EXPECT_EQ(ccm::gen::cos_gen<double>(-0.0), 1.0);						   // cos(-0) = 1
	EXPECT_EQ(d_bits(ccm::gen::tan_gen<double>(-0.0)), 0x8000000000000000ULL);  // tan(-0) = -0
	EXPECT_EQ(f_bits(ccm::gen::sin_gen<float>(-0.0F)), 0x80000000U);
	EXPECT_EQ(ccm::gen::cos_gen<float>(-0.0F), 1.0F);
}
