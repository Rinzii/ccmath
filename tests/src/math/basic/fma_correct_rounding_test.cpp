/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// Correctly-rounded gate for the software fma. fma is defined to round the exact x*y + z
// once, so the ccmath fixed-accumulator kernel must agree bit for bit with std::fma (itself
// correctly rounded) under every rounding mode. These broad randomized and adversarial sweeps
// complement the curated structured cases in tests/conformance/fma_rounding_test.cpp: full
// random bit patterns, a near-cancellation generator that stresses the sticky/borrow logic,
// and the classic float double-rounding killers that a non-fused double evaluation gets wrong.

#include "ccmath/ccmath.hpp"
#include "utils/test_runtime.hpp"

#include <gtest/gtest.h>

#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <random>
#include <vector>

namespace
{
	using ccm::test::runtime_value;

	template <typename T>
	struct uint_for;
	template <>
	struct uint_for<float>
	{
		using type = std::uint32_t;
	};
	template <>
	struct uint_for<double>
	{
		using type = std::uint64_t;
	};

	template <typename T>
	typename uint_for<T>::type bits(T f)
	{
		typename uint_for<T>::type u{};
		std::memcpy(&u, &f, sizeof(u));
		return u;
	}

	template <typename T>
	T from_bits(typename uint_for<T>::type u)
	{
		T f{};
		std::memcpy(&f, &u, sizeof(f));
		return f;
	}

	template <typename T>
	bool same_result(T a, T b)
	{
		if (std::isnan(a) && std::isnan(b)) { return true; }
		return bits<T>(a) == bits<T>(b);
	}

	constexpr int kModes[4]		   = { FE_TONEAREST, FE_TOWARDZERO, FE_UPWARD, FE_DOWNWARD };
	constexpr const char* kName[4] = { "FE_TONEAREST", "FE_TOWARDZERO", "FE_UPWARD", "FE_DOWNWARD" };

	template <typename T>
	T ccm_fma(T x, T y, T z)
	{ return ccm::fma(runtime_value(x), runtime_value(y), runtime_value(z)); }

	template <typename T>
	T std_fma(T x, T y, T z)
	{ return static_cast<T (*)(T, T, T)>(std::fma)(runtime_value(x), runtime_value(y), runtime_value(z)); }

	template <typename T>
	void expect_cr(const std::vector<T>& xs, const std::vector<T>& ys, const std::vector<T>& zs)
	{
		const int saved = std::fegetround();
		long failures	= 0;
		for (int m = 0; m < 4 && failures < 10; ++m)
		{
			for (std::size_t i = 0; i < xs.size(); ++i)
			{
				std::fesetround(kModes[m]);
				const T got = ccm_fma<T>(xs[i], ys[i], zs[i]);
				const T ref = std_fma<T>(xs[i], ys[i], zs[i]);
				std::fesetround(saved);
				if (!same_result<T>(got, ref))
				{
					SCOPED_TRACE(kName[m]);
					EXPECT_EQ(bits<T>(got), bits<T>(ref)) << "x=" << xs[i] << " y=" << ys[i] << " z=" << zs[i];
					if (++failures >= 10) { break; }
				}
			}
		}
		std::fesetround(saved);
	}

	template <typename T>
	void gen_random(std::vector<T>& xs, std::vector<T>& ys, std::vector<T>& zs, std::size_t n, std::uint64_t seed)
	{
		std::mt19937_64 rng(seed);
		using U = typename uint_for<T>::type;
		for (std::size_t i = 0; i < n; ++i)
		{
			xs.push_back(from_bits<T>(static_cast<U>(rng())));
			ys.push_back(from_bits<T>(static_cast<U>(rng())));
			zs.push_back(from_bits<T>(static_cast<U>(rng())));
		}
	}

	// z chosen near -(x*y) to stress cancellation, sticky bits, and the borrow path.
	template <typename T>
	void gen_cancellation(std::vector<T>& xs, std::vector<T>& ys, std::vector<T>& zs, std::size_t n, std::uint64_t seed)
	{
		std::mt19937_64 rng(seed);
		std::uniform_real_distribution<double> mantissa(-2.0, 2.0);
		std::uniform_int_distribution<int> scale(-40, 40);
		for (std::size_t i = 0; i < n; ++i)
		{
			const T a		  = static_cast<T>(std::ldexp(mantissa(rng), scale(rng)));
			const T b		  = static_cast<T>(std::ldexp(mantissa(rng), scale(rng)));
			const double prod = -static_cast<double>(a) * static_cast<double>(b);
			const int jitter  = std::numeric_limits<T>::digits + static_cast<int>(rng() % 6);
			const T c		  = static_cast<T>(prod * (1.0 + std::ldexp(mantissa(rng), -jitter)));
			xs.push_back(a);
			ys.push_back(b);
			zs.push_back(c);
		}
	}
} // namespace

TEST(CcmathFmaCorrectRounding, FloatRandomAllModes)
{
	std::vector<float> xs, ys, zs;
	gen_random<float>(xs, ys, zs, 60000, 0xF10A7);
	gen_cancellation<float>(xs, ys, zs, 40000, 0xCA11);
	// Classic float double-rounding killers that double(x)*double(y)+double(z) gets wrong.
	const float kk[][3] = {
		{ 1.0F + 0x1p-12F, 1.0F + 0x1p-12F, 0x1p-53F },
		{ 1.0F + 0x1p-12F, 1.0F + 0x1p-12F, -0x1p-53F },
		{ 1.0F - 0x1p-13F, 1.0F - 0x1p-13F, 0x1p-54F },
		{ 1.0F + 0x1p-11F, 1.0F + 0x1p-13F, 0x1p-52F },
	};
	for (const auto& k : kk)
	{
		xs.push_back(k[0]);
		ys.push_back(k[1]);
		zs.push_back(k[2]);
	}
	expect_cr<float>(xs, ys, zs);
}

TEST(CcmathFmaCorrectRounding, DoubleRandomAllModes)
{
	std::vector<double> xs, ys, zs;
	gen_random<double>(xs, ys, zs, 60000, 0xD00B1E);
	gen_cancellation<double>(xs, ys, zs, 40000, 0x5EED);
	expect_cr<double>(xs, ys, zs);
}
