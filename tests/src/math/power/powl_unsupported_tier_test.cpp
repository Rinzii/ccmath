/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "ccmath/ccmath.hpp"
#include "ccmath/internal/config/long_double_format.hpp"
#include "ccmath/internal/config/powl_policy.hpp"
#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"
#include "oracle/powl_path_reporting.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <iostream>

// Call detect_long_double_format() directly in the condition rather than binding it to a const
// local. A const local initialized from a constexpr function reads as a compile-time constant to
// MSVC /W4 (C4127), while if constexpr here would make the post-skip body unreachable (C4702).
#define REQUIRE_POWL_UNSUPPORTED_TIER()                                                                                                                        \
	do                                                                                                                                                         \
	{                                                                                                                                                          \
		if (ccm::config::detect_long_double_format() != ccm::config::LongDoubleFormat::IEEEBinary128 &&                                                        \
			ccm::config::detect_long_double_format() != ccm::config::LongDoubleFormat::Unknown)                                                                \
		{                                                                                                                                                      \
			GTEST_SKIP() << "binary128 or unknown long double required";                                                                                       \
		}                                                                                                                                                      \
	} while (false)

TEST(PowlUnsupportedTier, DetectsBinary128OrUnknown)
{
	REQUIRE_POWL_UNSUPPORTED_TIER();
	const auto format = ccm::config::detect_long_double_format();

	std::cout << "powl unsupported tier smoke: format=" << ccm::config::long_double_format_name(format)
			  << " fallback=" << (ccm::config::reduced_precision_powl_fallback_enabled() ? "enabled" : "disabled") << '\n';
	EXPECT_TRUE(format == ccm::config::LongDoubleFormat::IEEEBinary128 || format == ccm::config::LongDoubleFormat::Unknown);
}

TEST(PowlUnsupportedTier, PolicySelectsFallbackOrUnsupported)
{
	REQUIRE_POWL_UNSUPPORTED_TIER();

	const long double base	   = 2.0L;
	const long double exponent = 3.0L;
	const auto path			   = ccm::test::oracle::classify_powl_gen_path(base, exponent);

	if (ccm::config::reduced_precision_powl_fallback_enabled())
	{
		EXPECT_TRUE(path == ccm::test::oracle::PowlImplementationPath::Ld128ReducedPrecisionFallback ||
					path == ccm::test::oracle::PowlImplementationPath::UnknownReducedPrecisionFallback);
		EXPECT_TRUE(ccm::test::oracle::powl_path_is_reduced_precision(path));
		const long double actual   = ccm::gen::pow_gen(base, exponent);
		const long double expected = static_cast<long double>(ccm::gen::pow_gen(static_cast<double>(base), static_cast<double>(exponent)));
		EXPECT_EQ(actual, expected);
	} else
	{
		EXPECT_TRUE(path == ccm::test::oracle::PowlImplementationPath::Ld128Unsupported ||
					path == ccm::test::oracle::PowlImplementationPath::UnknownUnsupported);
		EXPECT_TRUE(std::isnan(ccm::gen::pow_gen(base, exponent)));
	}
}
