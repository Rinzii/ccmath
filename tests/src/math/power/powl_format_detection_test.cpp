/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "ccmath/internal/config/long_double_format.hpp"
#include "ccmath/internal/config/type_support.hpp"

#include <gtest/gtest.h>

#include <limits>
#include <string_view>

namespace
{
	void ExpectMacroMatchesFormat(ccm::config::LongDoubleFormat expected)
	{
		EXPECT_EQ(ccm::config::detect_long_double_format(), expected);
		EXPECT_STREQ(ccm::config::long_double_format_name(expected), ccm::config::long_double_format_name(ccm::config::detect_long_double_format()));
	}
} // namespace

TEST(PowlFormatDetection, NumericLimitsMatchExpectedTier)
{
	const int digits	   = std::numeric_limits<long double>::digits;
	const int max_exponent = std::numeric_limits<long double>::max_exponent;

	std::cout << "long double format: digits=" << digits << " max_exponent=" << max_exponent
			  << " name=" << ccm::config::long_double_format_name(ccm::config::detect_long_double_format()) << '\n';

	EXPECT_GT(digits, 0);
	EXPECT_GT(max_exponent, 0);

#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT64)
	EXPECT_EQ(digits, 53);
	EXPECT_EQ(max_exponent, 1024);
	ExpectMacroMatchesFormat(ccm::config::LongDoubleFormat::Double);
#elif defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
	EXPECT_EQ(digits, 64);
	EXPECT_EQ(max_exponent, 16384);
	ExpectMacroMatchesFormat(ccm::config::LongDoubleFormat::X87Extended);
#elif defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT128)
	EXPECT_EQ(digits, 113);
	EXPECT_EQ(max_exponent, 16384);
	ExpectMacroMatchesFormat(ccm::config::LongDoubleFormat::IEEEBinary128);
#else
	ExpectMacroMatchesFormat(ccm::config::LongDoubleFormat::Unknown);
#endif
}

TEST(PowlFormatDetection, StableFormatNameIsNonEmpty)
{
	const char * name = ccm::config::long_double_format_name(ccm::config::detect_long_double_format());
	ASSERT_NE(name, nullptr);
	EXPECT_GT(std::string_view{ name }.size(), 0u);
}
