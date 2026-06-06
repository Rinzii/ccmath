/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <array>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <type_traits>

#include "ccmath/ccmath.hpp"
#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "tests/utils/math_samples.hpp"
#include "tests/utils/ulp_distance.hpp"

using namespace ccm::support::fp;

namespace
{
std::string compiler_id()
{
#if defined(__clang__)
	return std::string("clang ") + __clang_version__;
#elif defined(__GNUC__)
	return std::string("gcc ") + __VERSION__;
#elif defined(_MSC_FULL_VER)
	return "msvc " + std::to_string(_MSC_FULL_VER);
#elif defined(_MSC_VER)
	return "msvc " + std::to_string(_MSC_VER);
#else
	return "unknown";
#endif
}

const char * target_arch()
{
#if defined(__aarch64__) || defined(_M_ARM64)
	return "aarch64";
#elif defined(__x86_64__) || defined(_M_X64)
	return "x86_64";
#elif defined(__arm__) || defined(_M_ARM)
	return "arm";
#elif defined(__i386__) || defined(_M_IX86)
	return "x86";
#else
	return "unknown";
#endif
}

const char * rounding_mode_name(int mode)
{
	switch (mode)
	{
	case FE_TONEAREST: return "FE_TONEAREST";
	case FE_DOWNWARD: return "FE_DOWNWARD";
	case FE_UPWARD: return "FE_UPWARD";
	case FE_TOWARDZERO: return "FE_TOWARDZERO";
	default: return "unknown";
	}
}

const char * fast_math_status()
{
#if defined(__FAST_MATH__)
	return "on";
#else
	return "off";
#endif
}
} // namespace

template <typename T>
int64_t ulp_difference_local(T a, T b)
{
	const std::uint64_t distance = ccm::test::ulp::distance_or_max(a, b);
	return (distance > static_cast<std::uint64_t>(std::numeric_limits<int64_t>::max()))
			   ? std::numeric_limits<int64_t>::max()
			   : static_cast<int64_t>(distance);
}

struct Stats
{
	int64_t min = std::numeric_limits<int64_t>::max();
	int64_t max = 0;
	long double sum = 0.0L;
	std::size_t count = 0;

	void add(int64_t ulp)
	{
		if (ulp < min) { min = ulp; }
		if (ulp > max) { max = ulp; }
		sum += static_cast<long double>(ulp);
		++count;
	}

	long double avg() const
	{
		return count == 0 ? 0.0L : (sum / static_cast<long double>(count));
	}
};

template <typename T, typename Fn, typename RefFn>
Stats measure_suite(const std::array<T, 6> & bases, const std::array<T, 6> & exponents, Fn fn, RefFn ref)
{
	Stats stats;
	for (std::size_t i = 0; i < bases.size(); ++i)
	{
		const T expected = ref(bases[i], exponents[i]);
		if (std::isnan(expected)) { continue; }
		stats.add(ulp_difference_local(fn(bases[i], exponents[i]), expected));
	}
	return stats;
}

template <typename T, std::size_t NB, std::size_t NE, typename Fn, typename RefFn>
Stats measure_grid(const std::array<T, NB> & bases, const std::array<T, NE> & exponents, Fn fn, RefFn ref)
{
	Stats stats;
	for (T base : bases)
	{
		for (T exponent : exponents)
		{
			const T expected = ref(base, exponent);
			if (std::isnan(expected)) { continue; }
			stats.add(ulp_difference_local(fn(base, exponent), expected));
		}
	}
	return stats;
}

template <typename T, typename Fn, typename RefFn>
Stats measure_random(Fn fn, RefFn ref)
{
	Stats stats;
	std::mt19937_64 rng(0xCCBEEF1234ULL);
	std::uniform_real_distribution<long double> base_dist(0.01L, 1000.0L);
	std::uniform_real_distribution<long double> exponent_dist(-16.0L, 16.0L);

	for (std::size_t i = 0; i < 200000; ++i)
	{
		const T base = static_cast<T>(base_dist(rng));
		const T exponent = static_cast<T>(exponent_dist(rng));
		const T expected = ref(base, exponent);
		if (std::isnan(expected)) { continue; }
		stats.add(ulp_difference_local(fn(base, exponent), expected));
	}

	return stats;
}

template <typename T>
void print_stats(const char * label, const Stats & stats)
{
	std::cout << label << ": count=" << stats.count << " min=" << (stats.count == 0 ? 0 : stats.min) << " max=" << stats.max << " avg="
			  << std::fixed << std::setprecision(9) << static_cast<double>(stats.avg()) << '\n';
}

int main()
{
	std::cout << "compiler=" << compiler_id() << '\n';
	std::cout << "target_arch=" << target_arch() << '\n';
	std::cout << "rounding_mode=" << rounding_mode_name(std::fegetround()) << '\n';
	std::cout << "fast_math=" << fast_math_status() << '\n';
	std::cout << "sizeof(long double)=" << sizeof(long double) << " digits=" << std::numeric_limits<long double>::digits << '\n';

	const auto float_suite = measure_suite<float>(ccm::test::samples::kPowFloatPairsBase,
												  ccm::test::samples::kPowFloatPairsExp,
												  [](float base, float exponent) { return ccm::powf(base, exponent); },
												  [](float base, float exponent) { return std::pow(base, exponent); });
	print_stats<float>("powf suite", float_suite);

	const auto float_generic = measure_random<float>([](float base, float exponent) { return ccm::gen::pow_gen(base, exponent); },
													 [](float base, float exponent) { return std::pow(base, exponent); });
	print_stats<float>("pow_gen<float> random", float_generic);

	const auto double_suite = measure_grid<double>(ccm::test::samples::kPowBases,
												   ccm::test::samples::kPowExponents,
												   [](double base, double exponent) { return ccm::pow(base, exponent); },
												   [](double base, double exponent) { return std::pow(base, exponent); });
	print_stats<double>("pow<double> suite", double_suite);

	const auto double_generic = measure_random<double>([](double base, double exponent) { return ccm::gen::pow_gen(base, exponent); },
													   [](double base, double exponent) { return std::pow(base, exponent); });
	print_stats<double>("pow_gen<double> random", double_generic);

	const auto long_double_suite = measure_grid<long double>(std::array<long double, 8>{ 0.25L, 0.5L, 1.0L, 2.0L, 3.0L, 4.0L, 10.0L, 14.0L },
															 std::array<long double, 9>{ -2.0L, -1.0L, -0.5L, 0.0L, 0.5L, 1.0L, 2.0L, 3.0L, 4.0L },
															 [](long double base, long double exponent) { return ccm::powl(base, exponent); },
															 [](long double base, long double exponent) { return std::pow(base, exponent); });
	print_stats<long double>("powl suite", long_double_suite);

	const auto long_double_generic = measure_random<long double>([](long double base, long double exponent) { return ccm::gen::pow_gen(base, exponent); },
																 [](long double base, long double exponent) { return std::pow(base, exponent); });
	print_stats<long double>("pow_gen<long double> random", long_double_generic);

	return 0;
}
