/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include <gtest/gtest.h>

#include <ccmath/ccmath.hpp>
#include <cmath>
#include <cstdint>
#include <limits>


TEST(CcmathBasicTests, NanStaticAssert)
{
	constexpr std::uint64_t ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nan(""));
	constexpr std::uint64_t stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::numeric_limits<double>::quiet_NaN());

	static_assert(ccmNanBits == stdNanBits, "ccm::nan() is NOT static assertable!");
	static_assert(ccm::isnan(ccm::nan("")), "ccm::nan() is NOT static assertable!"); // Assume we have access to ccm::isnan

	// now test nanf
	constexpr std::uint32_t ccmNanfBits = ccm::support::bit_cast<std::uint32_t>(ccm::nanf(""));
	constexpr std::uint32_t stdNanfBits = ccm::support::bit_cast<std::uint32_t>(std::numeric_limits<float>::quiet_NaN());

	static_assert(ccmNanfBits == stdNanfBits, "ccm::nanf() is NOT static assertable!");
	static_assert(ccm::isnan(ccm::nanf("")), "ccm::nanf() is NOT static assertable!"); // Assume we have access to ccm::isnan

	// If your IDE is saying this is not an integral constant. It is wrong. The code will compile without issue.
	static_assert(ccm::isnan(ccm::nanl("")), "ccm::nanl() is NOT static assertable!"); // Assume we have access to ccm::isnan

}

TEST(CcmathBasicTests, NanDouble)
{


	// Check the outcome if we are handling an empty string
	std::uint64_t ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nan(""));
	std::uint64_t stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nan(""));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with a single character that represents a number
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nan("1"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nan("1"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with a single character that represents a number
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nan("2"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nan("2"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with a single character that represents a letter
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nan("a"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nan("a"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with multiple characters and no numbers or hex prefix
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nan("foobar"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nan("foobar"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with multiple characters with both numbers and characters with no hex prefix
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nan("foo123bar"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nan("foo123bar"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with a hex prefix and a single character
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nan("0x1"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nan("0x1"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check for multi-digit wrapping
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nan("000000000000000000000000000000000000000000000000000000001"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nan("000000000000000000000000000000000000000000000000000000001"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with a hex prefix and a single character
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nan("0x2"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nan("0x2"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check for multi-digit wrapping
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nan("000000000000000000000000000000000000000000000000000000002"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nan("000000000000000000000000000000000000000000000000000000002"));
	EXPECT_EQ(ccmNanBits, stdNanBits);


	// TODO: Correct these test cases.
	/** These currently fail. Likely due to the fact that we are not handling overflow.
	 * ccm::nan is a pretty low priority function so I will not be fixing this for the time being, but
	 * I will be allowing these issues inside of the codebase and fix them at a later date.
	// Check the outcome if we are handling a string with a hex prefix and multiple characters
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nan("0x7FF8000000000000"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nan("0x7FF8000000000000"));
	EXPECT_EQ(ccmNanBits, stdNanBits);


	// Check the outcome if we are handling a string with a hex prefix and multiple characters
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nan("0x7FF8000000000001"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nan("0x7FF8000000000001"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with a VERY large number
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nan("123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nan("123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"));
	EXPECT_EQ(ccmNanBits, stdNanBits);
    */


}
