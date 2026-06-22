/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// The vectorized powf kernel is contractually bit identical to the scalar correctly rounded
// powf_impl on every lane. These tests pin that contract across several vector widths and over
// the normal range, the hard near-one region, integer exponents, the exceptional grid, and the
// overflow/underflow edges. The kernel runs in any build (pp deduces its own ABIs), so no
// runtime-simd configuration is required here.

#include "ccmath/internal/math/generic/func/power/pow_impl/powf_impl.hpp"
#include "ccmath/internal/math/generic/func/power/pow_impl/powf_simd_impl.hpp"

#include <gtest/gtest.h>

#include <algorithm>
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

	std::uint32_t float_bits(float value)
	{
		std::uint32_t out{};
		std::memcpy(&out, &value, sizeof(out));
		return out;
	}

	// NaN payloads are not part of the contract, so any NaN matches any NaN.
	bool bit_equal(float a, float b)
	{
		if (std::isnan(a) && std::isnan(b)) { return true; }
		return float_bits(a) == float_bits(b);
	}

	std::pair<std::vector<float>, std::vector<float>> build_inputs()
	{
		std::vector<float> xs;
		std::vector<float> ys;
		const auto add = [&](float x, float y)
		{
			xs.push_back(x);
			ys.push_back(y);
		};

		const float inf	   = std::numeric_limits<float>::infinity();
		const float qnan   = std::numeric_limits<float>::quiet_NaN();
		const float denorm = std::numeric_limits<float>::denorm_min();

		// Exceptional grid: zeros, ones, powers of two, infinities, NaN, denormals, negative bases.
		const float special_bases[] = { 0.0f, -0.0f, 1.0f, -1.0f, 2.0f, -2.0f, 4.0f, 10.0f, 0.5f, inf, -inf, qnan, denorm, -denorm, 3.4e38f, -3.4e38f };
		const float special_exps[]	= { 0.0f, -0.0f, 1.0f, -1.0f, 2.0f, 3.0f, -3.0f, 0.5f, -0.5f, 2.5f, inf, -inf, qnan, 127.0f, -149.0f, 0.3333333f };
		for (float b : special_bases)
		{
			for (float e : special_exps) { add(b, e); }
		}

		// Normal range grid.
		for (float b = 0.125f; b <= 64.0f; b *= 1.3f)
		{
			for (float e = -18.0f; e <= 18.0f; e += 0.7f) { add(b, e); }
		}

		// Hard near-one region with large exponents (stresses the Ziv resolution).
		for (float b = 0.9f; b <= 1.1f; b += 0.0007f)
		{
			for (float e : { -240.0f, -50.0f, -7.5f, 7.5f, 50.0f, 240.0f }) { add(b, e); }
		}

		// Integer exponents (exact integer power fast paths) and negative bases.
		for (float b = -12.0f; b <= 12.0f; b += 0.5f)
		{
			for (int e = -16; e <= 16; ++e) { add(b, static_cast<float>(e)); }
		}

		// Overflow and underflow edges.
		for (float b = 2.0f; b <= 100.0f; b += 1.5f)
		{
			const float k = std::log(3.0e38f) / std::log(b);
			for (float d : { -1.5f, -0.25f, 0.0f, 0.25f, 1.5f })
			{
				add(b, k + d);
				add(b, -(k + d));
			}
		}

		// Deterministic pseudo-random fill spanning a wide dynamic range.
		std::mt19937 rng(20240611U);
		std::uniform_real_distribution<float> base_dist(1.0e-6f, 1.0e6f);
		std::uniform_real_distribution<float> exp_dist(-60.0f, 60.0f);
		for (int i = 0; i < 40000; ++i) { add(base_dist(rng), exp_dist(rng)); }

		return { std::move(xs), std::move(ys) };
	}

	template <typename V>
	void expect_matches_scalar(const std::vector<float>& xs, const std::vector<float>& ys, const char* tag)
	{
		constexpr int width = static_cast<int>(V::size());
		ASSERT_EQ(xs.size(), ys.size());

		for (std::size_t base = 0; base < xs.size(); base += width)
		{
			alignas(64) float xb[64];
			alignas(64) float yb[64];
			const int count = static_cast<int>(std::min<std::size_t>(width, xs.size() - base));
			for (int i = 0; i < width; ++i)
			{
				const bool real = i < count;
				xb[i]			= real ? xs[base + static_cast<std::size_t>(i)] : 1.0f;
				yb[i]			= real ? ys[base + static_cast<std::size_t>(i)] : 1.0f;
			}

			const V xv(xb, element_aligned);
			const V yv(yb, element_aligned);
			const V rv = ccm::gen::impl::powf_simd(xv, yv);

			for (int i = 0; i < count; ++i)
			{
				const float got = rv[i];
				const float ref = ccm::gen::impl::powf_impl(xb[i], yb[i]);
				EXPECT_TRUE(bit_equal(got, ref)) << tag << " pow(" << xb[i] << ", " << yb[i] << ") got=0x" << std::hex << float_bits(got) << " ref=0x"
												 << float_bits(ref);
			}
		}
	}

	template <typename V>
	float scalar_lane(float x, float y)
	{ return ccm::gen::impl::powf_simd(V(x), V(y))[0]; }
} // namespace

TEST(CcmathPowfSimd, BitIdenticalToScalarKernelNativeWidth)
{
	const auto [xs, ys] = build_inputs();
	expect_matches_scalar<native_simd<float>>(xs, ys, "native");
}

TEST(CcmathPowfSimd, BitIdenticalToScalarKernelWidth4)
{
	const auto [xs, ys] = build_inputs();
	expect_matches_scalar<simd<float, 4>>(xs, ys, "w4");
}

TEST(CcmathPowfSimd, BitIdenticalToScalarKernelWidth8)
{
	if constexpr (VecAbi<8>::template IsValid<float>::value && VecAbi<8>::template IsValid<double>::value)
	{
		const auto [xs, ys] = build_inputs();
		expect_matches_scalar<simd<float, 8>>(xs, ys, "w8");
	}
	else
	{
		GTEST_SKIP() << "8-wide float/double simd not available on this target";
	}
}

TEST(CcmathPowfSimd, ExactAndExceptionalValues)
{
	using V = native_simd<float>;

	EXPECT_EQ(scalar_lane<V>(2.0f, 10.0f), 1024.0f);
	EXPECT_EQ(scalar_lane<V>(2.0f, 3.0f), 8.0f);
	EXPECT_EQ(scalar_lane<V>(9.0f, 0.5f), 3.0f);
	EXPECT_EQ(scalar_lane<V>(4.0f, 0.5f), 2.0f);
	EXPECT_EQ(scalar_lane<V>(10.0f, 2.0f), 100.0f);
	EXPECT_EQ(scalar_lane<V>(-2.0f, 3.0f), -8.0f);

	// pow(1, y) == 1 and pow(x, 0) == 1 for every argument.
	EXPECT_EQ(scalar_lane<V>(1.0f, std::numeric_limits<float>::infinity()), 1.0f);
	EXPECT_EQ(scalar_lane<V>(123.0f, 0.0f), 1.0f);

	// pow(0, positive) == 0 exactly, pow(0, negative) == +inf.
	EXPECT_EQ(scalar_lane<V>(0.0f, 2.0f), 0.0f);
	EXPECT_EQ(scalar_lane<V>(0.0f, -2.0f), std::numeric_limits<float>::infinity());

	// Negative base with a non-integer exponent is a domain error producing NaN.
	EXPECT_TRUE(std::isnan(scalar_lane<V>(-2.0f, 0.5f)));

	// Overflow to infinity and underflow to zero.
	EXPECT_EQ(scalar_lane<V>(10.0f, 40.0f), std::numeric_limits<float>::infinity());
	EXPECT_EQ(scalar_lane<V>(10.0f, -50.0f), 0.0f);
}
