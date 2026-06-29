/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// The vectorized double precision pow kernel is contractually bit identical to the scalar
// correctly rounded pow_impl on every lane. These tests pin that contract across several
// vector widths and over the regions with distinct behavior: the in-range double-double
// fast path, the integer and half-integer exact paths, exceptional operands, subnormal
// bases, the over/underflow scale region, and huge exponents around the clamp threshold.
// The errno and floating point exception side effects are pinned against the scalar too.

#include "ccmath/internal/math/generic/func/power/pow_impl/pow_impl.hpp"
#include "ccmath/internal/math/generic/func/power/pow_impl/pow_simd_impl.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cerrno>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <random>
#include <utility>
#include <vector>

namespace
{
	using namespace ccm::pp;

	std::uint64_t double_bits(double value)
	{
		std::uint64_t out{};
		std::memcpy(&out, &value, sizeof(out));
		return out;
	}

	// NaN payloads are not part of the contract, so any NaN matches any NaN.
	bool bit_equal(double a, double b)
	{
		if (std::isnan(a) && std::isnan(b))
		{
			return true;
		}
		return double_bits(a) == double_bits(b);
	}

	std::pair<std::vector<double>, std::vector<double>> build_inputs()
	{
		std::vector<double> xs;
		std::vector<double> ys;
		const auto add = [&](double x, double y) {
			xs.push_back(x);
			ys.push_back(y);
		};

		const double inf	= std::numeric_limits<double>::infinity();
		const double qnan	= std::numeric_limits<double>::quiet_NaN();
		const double denorm = std::numeric_limits<double>::denorm_min();
		const double minn	= std::numeric_limits<double>::min();
		const double maxd	= std::numeric_limits<double>::max();

		// Exceptional grid: zeros, ones, infinities, NaN, denormals, extremes, negative bases.
		const double special_bases[] = {
			0.0, -0.0, 1.0, -1.0, 2.0, -2.0, 10.0, 0.5, inf, -inf, qnan, denorm, -denorm, minn, maxd, -maxd, 1.0000000000000002, 0.9999999999999999, 1.5, -1.5
		};
		const double special_exps[] = {
			0.0, -0.0, 1.0, -1.0, 2.0, 3.0, -3.0, 0.5, -0.5, 2.5, 1023.0, -1074.0, 1025.0, inf, -inf, qnan, 1e300, -1e300, 0.3333333333333333, 5.5e18
		};
		for (double b : special_bases)
		{
			for (double e : special_exps)
			{
				add(b, e);
			}
		}

		// Normal range grid through the vector fast path.
		for (double b = 0.001; b <= 1000.0; b *= 1.7)
		{
			for (double e = -150.0; e <= 150.0; e += 1.7)
			{
				add(b, e + 0.123456789);
			}
		}

		// Hard near-one region with large exponents (stresses the double-double pipeline).
		for (double b = 0.95; b <= 1.05; b += 0.00037)
		{
			for (double e : { -4000.1, -750.3, -13.7, 13.7, 750.3, 4000.1 })
			{
				add(b, e);
			}
		}

		// Integer exponents (exact ipow path) including negative bases.
		for (double b = -10.0; b <= 10.0; b += 0.375)
		{
			for (int e = -24; e <= 24; ++e)
			{
				add(b, static_cast<double>(e));
			}
		}

		// Half-integer exponents (exact sqrt path) and just-off neighbors (vector path).
		for (double b = 0.05; b <= 50.0; b *= 1.9)
		{
			for (int k = -19; k <= 19; ++k)
			{
				const double he = static_cast<double>(k) * 0.5;
				add(b, he);
				add(b, he + 0x1.0p-30);
				add(b, he - 0x1.0p-30);
			}
		}

		// Overflow and underflow edges (the clamped scale region).
		for (double b = 2.0; b <= 500.0; b *= 1.6)
		{
			const double k = std::log(1.7e308) / std::log(b);
			for (double d : { -2.0, -0.5, 0.0, 0.5, 2.0 })
			{
				add(b, k + d);
				add(b, -(k + d));
			}
		}

		// Subnormal bases.
		for (int e2 = -1074; e2 <= -1000; e2 += 7)
		{
			for (double e : { -2.5, -1.1, 0.7, 1.3, 2.9 })
			{
				add(std::ldexp(1.2345, e2), e);
			}
		}

		// Huge exponents straddling the clamp threshold 0x43d74910d52d3052.
		for (double b : { 0.97, 0.9999999, 1.0000001, 1.03 })
		{
			for (int e2 = 55; e2 <= 70; ++e2)
			{
				add(b, std::ldexp(1.3, e2));
				add(b, -std::ldexp(1.3, e2));
			}
		}

		// Deterministic pseudo-random fill spanning a wide dynamic range.
		std::mt19937_64 rng(20260612ULL);
		std::uniform_real_distribution<double> base_dist(1.0e-12, 1.0e12);
		std::uniform_real_distribution<double> exp_dist(-600.0, 600.0);
		for (int i = 0; i < 40000; ++i)
		{
			add(base_dist(rng), exp_dist(rng));
		}

		return { std::move(xs), std::move(ys) };
	}

	template <typename V> void expect_matches_scalar(const std::vector<double> & xs, const std::vector<double> & ys, const char * tag)
	{
		constexpr int width = static_cast<int>(V::size());
		ASSERT_EQ(xs.size(), ys.size());

		for (std::size_t base = 0; base < xs.size(); base += width)
		{
			alignas(64) double xb[64];
			alignas(64) double yb[64];
			const int count = static_cast<int>(std::min<std::size_t>(width, xs.size() - base));
			for (int i = 0; i < width; ++i)
			{
				const bool real = i < count;
				xb[i]			= real ? xs[base + static_cast<std::size_t>(i)] : 1.5;
				yb[i]			= real ? ys[base + static_cast<std::size_t>(i)] : 0.25;
			}

			const V xv(xb, element_aligned);
			const V yv(yb, element_aligned);
			const V rv = ccm::gen::impl::pow_simd(xv, yv);

			for (int i = 0; i < count; ++i)
			{
				const double got = rv[i];
				const double ref = ccm::gen::impl::pow_impl(xb[i], yb[i]);
				EXPECT_TRUE(bit_equal(got, ref)) << tag << " pow(" << xb[i] << ", " << yb[i] << ") got=0x" << std::hex << double_bits(got) << " ref=0x"
												 << double_bits(ref);
			}
		}
	}

	template <typename V> double scalar_lane(double x, double y)
	{
		return ccm::gen::impl::pow_simd(V(x), V(y))[0];
	}

	struct FenvObservation
	{
		bool overflow;
		bool underflow;
		bool invalid;
		bool divbyzero;
		int err;
	};

	template <typename F> FenvObservation observe_fenv(F && run)
	{
		std::feclearexcept(FE_ALL_EXCEPT);
		errno = 0;
		run();
		return { std::fetestexcept(FE_OVERFLOW) != 0,
				 std::fetestexcept(FE_UNDERFLOW) != 0,
				 std::fetestexcept(FE_INVALID) != 0,
				 std::fetestexcept(FE_DIVBYZERO) != 0,
				 errno };
	}
} // namespace

TEST(CcmathPowSimd, BitIdenticalToScalarKernelNativeWidth)
{
	const auto [xs, ys] = build_inputs();
	expect_matches_scalar<native_simd<double>>(xs, ys, "native");
}

TEST(CcmathPowSimd, BitIdenticalToScalarKernelWidth2)
{
	const auto [xs, ys] = build_inputs();
	expect_matches_scalar<simd<double, 2>>(xs, ys, "w2");
}

TEST(CcmathPowSimd, BitIdenticalToScalarKernelWidth4)
{
	const auto [xs, ys] = build_inputs();
	expect_matches_scalar<simd<double, 4>>(xs, ys, "w4");
}

TEST(CcmathPowSimd, BitIdenticalToScalarKernelWidth8)
{
	if constexpr (VecAbi<8>::template IsValid<double>::value)
	{
		const auto [xs, ys] = build_inputs();
		expect_matches_scalar<simd<double, 8>>(xs, ys, "w8");
	} else
	{
		GTEST_SKIP() << "8-wide double simd not available on this target";
	}
}

TEST(CcmathPowSimd, ExactAndExceptionalValues)
{
	using V = native_simd<double>;

	EXPECT_EQ(scalar_lane<V>(2.0, 10.0), 1024.0);
	EXPECT_EQ(scalar_lane<V>(2.0, 3.0), 8.0);
	EXPECT_EQ(scalar_lane<V>(9.0, 0.5), 3.0);
	EXPECT_EQ(scalar_lane<V>(4.0, -1.0), 0.25);
	EXPECT_EQ(scalar_lane<V>(10.0, 16.0), 1.0e16);
	EXPECT_EQ(scalar_lane<V>(-2.0, 3.0), -8.0);
	EXPECT_EQ(scalar_lane<V>(-2.0, 2.0), 4.0);

	// pow(1, y) == 1 and pow(x, 0) == 1 for every argument, including NaN.
	EXPECT_EQ(scalar_lane<V>(1.0, std::numeric_limits<double>::infinity()), 1.0);
	EXPECT_EQ(scalar_lane<V>(1.0, std::numeric_limits<double>::quiet_NaN()), 1.0);
	EXPECT_EQ(scalar_lane<V>(std::numeric_limits<double>::quiet_NaN(), 0.0), 1.0);
	EXPECT_EQ(scalar_lane<V>(123.0, 0.0), 1.0);

	// Zero base sign rules and the pole.
	EXPECT_EQ(scalar_lane<V>(0.0, 2.0), 0.0);
	EXPECT_EQ(scalar_lane<V>(-0.0, 3.0), -0.0);
	EXPECT_TRUE(std::signbit(scalar_lane<V>(-0.0, 3.0)));
	EXPECT_EQ(scalar_lane<V>(0.0, -2.0), std::numeric_limits<double>::infinity());
	EXPECT_EQ(scalar_lane<V>(-0.0, -3.0), -std::numeric_limits<double>::infinity());

	// Infinite operands.
	EXPECT_EQ(scalar_lane<V>(std::numeric_limits<double>::infinity(), 2.0), std::numeric_limits<double>::infinity());
	EXPECT_EQ(scalar_lane<V>(-std::numeric_limits<double>::infinity(), 3.0), -std::numeric_limits<double>::infinity());
	EXPECT_EQ(scalar_lane<V>(-std::numeric_limits<double>::infinity(), 0.5), std::numeric_limits<double>::infinity());
	EXPECT_EQ(scalar_lane<V>(2.0, std::numeric_limits<double>::infinity()), std::numeric_limits<double>::infinity());
	EXPECT_EQ(scalar_lane<V>(0.5, std::numeric_limits<double>::infinity()), 0.0);
	EXPECT_EQ(scalar_lane<V>(2.0, -std::numeric_limits<double>::infinity()), 0.0);

	// Negative base with a non-integer exponent is a domain error producing NaN.
	EXPECT_TRUE(std::isnan(scalar_lane<V>(-2.0, 0.5)));

	// Overflow to infinity and underflow to zero.
	EXPECT_EQ(scalar_lane<V>(10.0, 400.0), std::numeric_limits<double>::infinity());
	EXPECT_EQ(scalar_lane<V>(10.0, -400.0), 0.0);
}

TEST(CcmathPowSimd, FenvAndErrnoParityWithScalarKernel)
{
	using V = native_simd<double>;

	const double cases[][2] = {
		{ 1e300, 3.0 },		 // overflow
		{ 1e-300, 3.0 },	 // underflow
		{ 2.0, 2000.0 },	 // overflow through the scale path
		{ 2.0, -2000.0 },	 // underflow through the scale path
		{ 0.0, 2.0 },		 // exact zero, no flags
		{ 0.0, -2.0 },		 // pole, divide by zero
		{ -2.0, 0.5 },		 // domain error, invalid
		{ 2.0, 3.0 },		 // clean exact
		{ 1.5, 7.25 },		 // clean inexact fast path
		{ 0.5, 2000.0 },	 // underflow
		{ 1.0000001, 1e15 }, // overflow from a near-one base
	};

	for (const auto & c : cases)
	{
		volatile double simd_out	   = 0.0;
		volatile double scal_out	   = 0.0;
		const FenvObservation simd_obs = observe_fenv([&] { simd_out = ccm::gen::impl::pow_simd(V(c[0]), V(c[1]))[0]; });
		const FenvObservation scal_obs = observe_fenv([&] { scal_out = ccm::gen::impl::pow_impl(c[0], c[1]); });

		EXPECT_EQ(simd_obs.overflow, scal_obs.overflow) << "FE_OVERFLOW differs for pow(" << c[0] << ", " << c[1] << ")";
		EXPECT_EQ(simd_obs.underflow, scal_obs.underflow) << "FE_UNDERFLOW differs for pow(" << c[0] << ", " << c[1] << ")";
		EXPECT_EQ(simd_obs.invalid, scal_obs.invalid) << "FE_INVALID differs for pow(" << c[0] << ", " << c[1] << ")";
		EXPECT_EQ(simd_obs.divbyzero, scal_obs.divbyzero) << "FE_DIVBYZERO differs for pow(" << c[0] << ", " << c[1] << ")";
		EXPECT_EQ(simd_obs.err, scal_obs.err) << "errno differs for pow(" << c[0] << ", " << c[1] << ")";
		EXPECT_TRUE(bit_equal(simd_out, scal_out)) << "value differs for pow(" << c[0] << ", " << c[1] << ")";
	}
}
