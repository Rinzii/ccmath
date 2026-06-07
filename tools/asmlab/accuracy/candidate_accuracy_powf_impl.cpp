/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 * Isolated powf_impl candidate accuracy probe. Compares one (base, exp) pair per
 * invocation against std::powf using the project ULP policy. Never replaces production
 * headers.
 */

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <string>

#include "utils/ulp_distance.hpp"

#ifdef ASMLAB_POWF_IMPL_CANDIDATE
#include "candidate.hpp"
#define ASMLAB_POWF_IMPL_CALL(base, exp) \
	::asmlab::variant::powf_impl::ASMLAB_VARIANT_NAME::powf_impl((base), (exp))
#else
#include "ccmath/internal/math/generic/func/power/pow_impl/powf_impl.hpp"
#define ASMLAB_POWF_IMPL_CALL(base, exp) ::ccm::gen::impl::powf_impl((base), (exp))
#endif

namespace
{

constexpr int64_t kMaxAllowedUlp = 4;

float parse_float(const char * text)
{
	if (std::strcmp(text, "nan") == 0 || std::strcmp(text, "NaN") == 0)
	{
		return std::numeric_limits<float>::quiet_NaN();
	}
	if (std::strcmp(text, "inf") == 0 || std::strcmp(text, "+inf") == 0)
	{
		return std::numeric_limits<float>::infinity();
	}
	if (std::strcmp(text, "-inf") == 0) { return -std::numeric_limits<float>::infinity(); }
	if (std::strcmp(text, "-0") == 0 || std::strcmp(text, "-0.0") == 0) { return -0.0F; }
	if (std::strcmp(text, "denorm_min") == 0) { return std::numeric_limits<float>::denorm_min(); }
	if (std::strcmp(text, "min") == 0) { return std::numeric_limits<float>::min(); }
	if (std::strcmp(text, "max") == 0) { return std::numeric_limits<float>::max(); }
	return static_cast<float>(std::strtod(text, nullptr));
}

bool same_signed_zero(float a, float b)
{
	return a == 0.0F && b == 0.0F && std::signbit(a) == std::signbit(b);
}

bool check_ulp_vs_std(float base, float exp, int64_t max_ulp)
{
	const float expected = std::pow(base, exp);
	const float actual	 = ASMLAB_POWF_IMPL_CALL(base, exp);

	if (std::isnan(expected))
	{
		if (!std::isnan(actual))
		{
			std::cerr << "expected NaN got " << actual << "\n";
			return false;
		}
		return true;
	}
	if (std::isinf(expected))
	{
		if (actual != expected)
		{
			std::cerr << "expected " << expected << " got " << actual << "\n";
			return false;
		}
		return true;
	}
	if (same_signed_zero(actual, expected)) { return true; }

	const auto dist = ccm::test::ulp::distance_or_max(actual, expected);
	if (dist > static_cast<std::uint64_t>(max_ulp))
	{
		std::cerr << "ulp=" << dist << " actual=" << actual << " expected=" << expected << "\n";
		return false;
	}
	return true;
}

bool check_special_vs_std(float base, float exp)
{
	const float expected = std::pow(base, exp);
	const float actual	 = ASMLAB_POWF_IMPL_CALL(base, exp);

	if (std::isnan(expected))
	{
		if (!std::isnan(actual))
		{
			std::cerr << "expected NaN got " << actual << "\n";
			return false;
		}
		return true;
	}
	if (std::isinf(expected))
	{
		if (actual != expected)
		{
			std::cerr << "expected " << expected << " got " << actual << "\n";
			return false;
		}
		return true;
	}
	if (same_signed_zero(actual, expected)) { return true; }
	if (actual == expected) { return true; }
	std::cerr << "special mismatch actual=" << actual << " expected=" << expected << "\n";
	return false;
}

bool check_domain_vs_std(float base, float exp)
{
	const float expected = std::pow(base, exp);
	const float actual	 = ASMLAB_POWF_IMPL_CALL(base, exp);
	if (std::isnan(expected))
	{
		if (!std::isnan(actual))
		{
			std::cerr << "domain expected NaN got " << actual << "\n";
			return false;
		}
		return true;
	}
	return check_special_vs_std(base, exp);
}

} // namespace

int main(int argc, char ** argv)
{
	std::string check = "ulp";
	float base		  = 0.0F;
	float exp		  = 0.0F;
	int64_t max_ulp	  = kMaxAllowedUlp;

	for (int i = 1; i < argc; ++i)
	{
		if (std::strcmp(argv[i], "--check") == 0 && i + 1 < argc)
		{
			check = argv[++i];
		}
		else if (std::strcmp(argv[i], "--base") == 0 && i + 1 < argc)
		{
			base = parse_float(argv[++i]);
		}
		else if (std::strcmp(argv[i], "--exp") == 0 && i + 1 < argc)
		{
			exp = parse_float(argv[++i]);
		}
		else if (std::strcmp(argv[i], "--max-ulp") == 0 && i + 1 < argc)
		{
			max_ulp = std::strtoll(argv[++i], nullptr, 10);
		}
	}

	bool ok = false;
	if (check == "ulp") { ok = check_ulp_vs_std(base, exp, max_ulp); }
	else if (check == "special") { ok = check_special_vs_std(base, exp); }
	else if (check == "domain") { ok = check_domain_vs_std(base, exp); }
	else
	{
		std::cerr << "unknown check: " << check << "\n";
		return 2;
	}

	if (ok)
	{
		std::cout << "PASS\n";
		return 0;
	}
	std::cout << "FAIL\n";
	return 1;
}
