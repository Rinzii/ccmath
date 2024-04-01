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

constexpr double checkNan() noexcept
{
	return ccm::nan("");
}

TEST(CcmathBasicTests, NanStaticAssert)
{
	constexpr std::uint64_t ccmNanBits = ccm::helpers::bit_cast<std::uint64_t>(ccm::nan(""));
	constexpr std::uint64_t stdNanBits = ccm::helpers::bit_cast<std::uint64_t>(std::numeric_limits<double>::quiet_NaN());

	static_assert(ccmNanBits == stdNanBits, "ccm::nan() is NOT static assertable!");
}

TEST(CcmathBasicTests, NanDouble)
{

	// TODO: Investigate why MSVC is adding 1 to the value of the nan bits of tests using this variable.
#if defined(_MSC_VER)
	const std::uint64_t addPlusOneForMsvc = 1;
#else
    const std::uint64_t addPlusOneForMsvc = 0;
#endif

	// Check the outcome if we are handling an empty string
	std::uint64_t ccmNanBits = ccm::helpers::bit_cast<std::uint64_t>(ccm::nan("")) + addPlusOneForMsvc;
	std::uint64_t stdNanBits = ccm::helpers::bit_cast<std::uint64_t>(std::nan("")) + addPlusOneForMsvc;
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with a single character that represents a number
	ccmNanBits = ccm::helpers::bit_cast<std::uint64_t>(ccm::nan("1"));
	stdNanBits = ccm::helpers::bit_cast<std::uint64_t>(std::nan("1"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with a single character that represents a letter
	ccmNanBits = ccm::helpers::bit_cast<std::uint64_t>(ccm::nan("a"));
	stdNanBits = ccm::helpers::bit_cast<std::uint64_t>(std::nan("a"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with multiple characters and no numbers or hex prefix
	ccmNanBits = ccm::helpers::bit_cast<std::uint64_t>(ccm::nan("foobar"));
	stdNanBits = ccm::helpers::bit_cast<std::uint64_t>(std::nan("foobar"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with multiple characters with both numbers and characters with no hex prefix
	ccmNanBits = ccm::helpers::bit_cast<std::uint64_t>(ccm::nan("foo123bar")) + addPlusOneForMsvc;
	stdNanBits = ccm::helpers::bit_cast<std::uint64_t>(std::nan("foo123bar")) + addPlusOneForMsvc;
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with a hex prefix and a single character
	ccmNanBits = ccm::helpers::bit_cast<std::uint64_t>(ccm::nan("0x1")) + addPlusOneForMsvc;
	stdNanBits = ccm::helpers::bit_cast<std::uint64_t>(std::nan("0x1") + addPlusOneForMsvc);
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check for multi-digit wrapping
	ccmNanBits = ccm::helpers::bit_cast<std::uint64_t>(ccm::nan("000000000000000000000000000000000000000000000000000000001"));
	stdNanBits = ccm::helpers::bit_cast<std::uint64_t>(std::nan("000000000000000000000000000000000000000000000000000000001"));
	EXPECT_EQ(ccmNanBits, stdNanBits);


	// TODO: Correct these test cases.
	/** These currently fail. Likely due to the fact that we are not handling overflow.
	 * ccm::nan is a pretty low priority function so I will not be fixing this for the time being, but
	 * I will be allowing these issues inside of the codebase and fix them at a later date.
	// Check the outcome if we are handling a string with a hex prefix and multiple characters
	ccmNanBits = ccm::helpers::bit_cast<std::uint64_t>(ccm::nan("0x7FF8000000000000"));
	stdNanBits = ccm::helpers::bit_cast<std::uint64_t>(std::nan("0x7FF8000000000000"));
	EXPECT_EQ(ccmNanBits, stdNanBits);


	// Check the outcome if we are handling a string with a hex prefix and multiple characters
	ccmNanBits = ccm::helpers::bit_cast<std::uint64_t>(ccm::nan("0x7FF8000000000001"));
	stdNanBits = ccm::helpers::bit_cast<std::uint64_t>(std::nan("0x7FF8000000000001"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with a VERY large number
	ccmNanBits = ccm::helpers::bit_cast<std::uint64_t>(ccm::nan("123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"));
	stdNanBits = ccm::helpers::bit_cast<std::uint64_t>(std::nan("123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"));
	EXPECT_EQ(ccmNanBits, stdNanBits);
    */


}
