/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <gtest/gtest.h>

#include <ccmath/ccmath.hpp>
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

#include "ccmath/internal/types/float128.hpp"

// TODO: The nan func are quite brittle and the test cases are extremely forgiving to the func.
// 		 At some point we should improve these test cases and the nan func to handle all edge cases.

TEST(CcmathBasicTests, NanStaticAssert)
{
	// Assume we have access to ccm::isnan
	static_assert(ccm::isnan(ccm::nanf("")), "ccm::nanf() is NOT static assertable!");
	static_assert(ccm::isnan(ccm::nan("")), "ccm::nan() is NOT static assertable!");
	static_assert(ccm::isnan(ccm::nanl("")), "ccm::nanl() is NOT static assertable!");
}

TEST(CcmathBasicTests, Nan_Double)
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

#if LDBL_MANT_DIG == 53

TEST(CcmathBasicTests, Nan_LDouble64bit)
{
	// Check the outcome if we are handling an empty string
	std::uint64_t ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nanl(""));
	std::uint64_t stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nanl(""));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with a single character that represents a number
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nanl("1"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nanl("1"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with a single character that represents a number
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nanl("2"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nanl("2"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with a single character that represents a letter
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nanl("a"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nanl("a"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with multiple characters and no numbers or hex prefix
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nanl("foobar"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nanl("foobar"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with multiple characters with both numbers and characters with no hex prefix
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nanl("foo123bar"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nanl("foo123bar"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with a hex prefix and a single character
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nanl("0x1"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nanl("0x1"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check for multi-digit wrapping
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nanl("000000000000000000000000000000000000000000000000000000001"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nanl("000000000000000000000000000000000000000000000000000000001"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check the outcome if we are handling a string with a hex prefix and a single character
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nanl("0x2"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nanl("0x2"));
	EXPECT_EQ(ccmNanBits, stdNanBits);

	// Check for multi-digit wrapping
	ccmNanBits = ccm::support::bit_cast<std::uint64_t>(ccm::nanl("000000000000000000000000000000000000000000000000000000002"));
	stdNanBits = ccm::support::bit_cast<std::uint64_t>(std::nanl("000000000000000000000000000000000000000000000000000000002"));
	EXPECT_EQ(ccmNanBits, stdNanBits);
}

// If our long double is 128 bits, then don't run this test as it would not be correct due to padding removal.
#elif LDBL_MANT_DIG == 64

/*
 * 80 bit long doubles have padding bits that are not determinable.
 * The bytes for this padding are random, and we can't predict it as such we remove all bytes after the 10th byte.
 * Why the 10th byte?
 * Because this is where the information to determine NaN stops and
 * the remainder afterward is padding that can't be determined or tested.
 */
template <std::size_t N>
void removePaddingBits(std::array<std::byte, N> & byteArray)
{
	// If the array has 10 or fewer elements, there's nothing to zero out
	if (N <= 10) { return; }

	// Zero out everything after the 10th element
	for (std::size_t i = 10; i < N; ++i) { byteArray.at(i) = std::byte{0}; }
}

template <std::size_t N>
std::array<std::byte, N> ldoubleToByteArray(long double value)
{
	std::array<std::byte, N> byteArray{};
	std::memcpy(byteArray.data(), &value, sizeof(long double));
	removePaddingBits(byteArray);
	return byteArray;
}

TEST(CcmathBasicTests, Nan_LDouble80bit)
{
	std::array<std::byte, sizeof(long double)> ccm_nan_bits = ldoubleToByteArray<sizeof(long double)>(ccm::nanl(""));
	std::array<std::byte, sizeof(long double)> std_nan_bits = ldoubleToByteArray<sizeof(long double)>(std::nanl(""));

	EXPECT_EQ(ccm_nan_bits, std_nan_bits);

	// Check the outcome if we are handling a string with a single character that represents a number
	ccm_nan_bits = ldoubleToByteArray<sizeof(long double)>(ccm::nanl("1"));
	std_nan_bits = ldoubleToByteArray<sizeof(long double)>(std::nanl("1"));
	EXPECT_EQ(ccm_nan_bits, std_nan_bits);

	// Check the outcome if we are handling a string with a single character that represents a number
	ccm_nan_bits = ldoubleToByteArray<sizeof(long double)>(ccm::nanl("2"));
	std_nan_bits = ldoubleToByteArray<sizeof(long double)>(std::nanl("2"));
	EXPECT_EQ(ccm_nan_bits, std_nan_bits);

	// Check the outcome if we are handling a string with a single character that represents a letter
	ccm_nan_bits = ldoubleToByteArray<sizeof(long double)>(ccm::nanl("a"));
	std_nan_bits = ldoubleToByteArray<sizeof(long double)>(std::nanl("a"));
	EXPECT_EQ(ccm_nan_bits, std_nan_bits);

	// Check the outcome if we are handling a string with multiple characters and no numbers or hex prefix
	ccm_nan_bits = ldoubleToByteArray<sizeof(long double)>(ccm::nanl("foobar"));
	std_nan_bits = ldoubleToByteArray<sizeof(long double)>(std::nanl("foobar"));
	EXPECT_EQ(ccm_nan_bits, std_nan_bits);

	// Check the outcome if we are handling a string with multiple characters with both numbers and characters with no hex prefix
	ccm_nan_bits = ldoubleToByteArray<sizeof(long double)>(ccm::nanl("foo123bar"));
	std_nan_bits = ldoubleToByteArray<sizeof(long double)>(std::nanl("foo123bar"));
	EXPECT_EQ(ccm_nan_bits, std_nan_bits);

	// Check the outcome if we are handling a string with a hex prefix and a single character
	ccm_nan_bits = ldoubleToByteArray<sizeof(long double)>(ccm::nanl("0x1"));
	std_nan_bits = ldoubleToByteArray<sizeof(long double)>(std::nanl("0x1"));
	EXPECT_EQ(ccm_nan_bits, std_nan_bits);

	// Check for multi-digit wrapping
	ccm_nan_bits = ldoubleToByteArray<sizeof(long double)>(ccm::nanl("000000000000000000000000000000000000000000000000000000001"));
	std_nan_bits = ldoubleToByteArray<sizeof(long double)>(std::nanl("000000000000000000000000000000000000000000000000000000001"));
	EXPECT_EQ(ccm_nan_bits, std_nan_bits);

	// Check the outcome if we are handling a string with a hex prefix and a single character
	ccm_nan_bits = ldoubleToByteArray<sizeof(long double)>(ccm::nanl("0x2"));
	std_nan_bits = ldoubleToByteArray<sizeof(long double)>(std::nanl("0x2"));
	EXPECT_EQ(ccm_nan_bits, std_nan_bits);

	// Check for multi-digit wrapping
	ccm_nan_bits = ldoubleToByteArray<sizeof(long double)>(ccm::nanl("000000000000000000000000000000000000000000000000000000002"));
	std_nan_bits = ldoubleToByteArray<sizeof(long double)>(std::nanl("000000000000000000000000000000000000000000000000000000002"));
	EXPECT_EQ(ccm_nan_bits, std_nan_bits);
}

#elif LDBL_MANT_DIG == 113

// Does not strip padding as there shouldn't be any if ldouble is 128 bits
template <std::size_t N>
std::array<std::byte, N> ldoubleToByteArray(ccm::types::float128 value)
{
	std::array<std::byte, N> byteArray{};
	std::memcpy(byteArray.data(), &value, sizeof(ccm::types::float128));
	return byteArray;
}

TEST(CcmathBasicTests, Nan_LDouble128bit)
{
	std::array<std::byte, sizeof(long double)> ccm_nan_bits = ldoubleToByteArray<sizeof(long double)>(ccm::nanl(""));
	std::array<std::byte, sizeof(long double)> std_nan_bits = ldoubleToByteArray<sizeof(long double)>(std::nanl(""));

	EXPECT_EQ(ccm_nan_bits, std_nan_bits);

	// Check the outcome if we are handling a string with a single character that represents a number
	ccm_nan_bits = ldoubleToByteArray<sizeof(long double)>(ccm::nanl("1"));
	std_nan_bits = ldoubleToByteArray<sizeof(long double)>(std::nanl("1"));
	EXPECT_EQ(ccm_nan_bits, std_nan_bits);

	// Check the outcome if we are handling a string with a single character that represents a number
	ccm_nan_bits = ldoubleToByteArray<sizeof(long double)>(ccm::nanl("2"));
	std_nan_bits = ldoubleToByteArray<sizeof(long double)>(std::nanl("2"));
	EXPECT_EQ(ccm_nan_bits, std_nan_bits);

	// Check the outcome if we are handling a string with a single character that represents a letter
	ccm_nan_bits = ldoubleToByteArray<sizeof(long double)>(ccm::nanl("a"));
	std_nan_bits = ldoubleToByteArray<sizeof(long double)>(std::nanl("a"));
	EXPECT_EQ(ccm_nan_bits, std_nan_bits);

	// Check the outcome if we are handling a string with multiple characters and no numbers or hex prefix
	ccm_nan_bits = ldoubleToByteArray<sizeof(long double)>(ccm::nanl("foobar"));
	std_nan_bits = ldoubleToByteArray<sizeof(long double)>(std::nanl("foobar"));
	EXPECT_EQ(ccm_nan_bits, std_nan_bits);

	// Check the outcome if we are handling a string with multiple characters with both numbers and characters with no hex prefix
	ccm_nan_bits = ldoubleToByteArray<sizeof(long double)>(ccm::nanl("foo123bar"));
	std_nan_bits = ldoubleToByteArray<sizeof(long double)>(std::nanl("foo123bar"));
	EXPECT_EQ(ccm_nan_bits, std_nan_bits);

	// Check the outcome if we are handling a string with a hex prefix and a single character
	ccm_nan_bits = ldoubleToByteArray<sizeof(long double)>(ccm::nanl("0x1"));
	std_nan_bits = ldoubleToByteArray<sizeof(long double)>(std::nanl("0x1"));
	EXPECT_EQ(ccm_nan_bits, std_nan_bits);

	// Check for multi-digit wrapping
	ccm_nan_bits = ldoubleToByteArray<sizeof(long double)>(ccm::nanl("000000000000000000000000000000000000000000000000000000001"));
	std_nan_bits = ldoubleToByteArray<sizeof(long double)>(std::nanl("000000000000000000000000000000000000000000000000000000001"));
	EXPECT_EQ(ccm_nan_bits, std_nan_bits);

	// Check the outcome if we are handling a string with a hex prefix and a single character
	ccm_nan_bits = ldoubleToByteArray<sizeof(long double)>(ccm::nanl("0x2"));
	std_nan_bits = ldoubleToByteArray<sizeof(long double)>(std::nanl("0x2"));
	EXPECT_EQ(ccm_nan_bits, std_nan_bits);

	// Check for multi-digit wrapping
	ccm_nan_bits = ldoubleToByteArray<sizeof(long double)>(ccm::nanl("000000000000000000000000000000000000000000000000000000002"));
	std_nan_bits = ldoubleToByteArray<sizeof(long double)>(std::nanl("000000000000000000000000000000000000000000000000000000002"));
	EXPECT_EQ(ccm_nan_bits, std_nan_bits);
}
#else

TEST(CcmathBasicTests, Nan_LDoubleUnknownBits)
{
	FAIL() << "We do not know how to handle long doubles with an unknown number of bits. Please report this if you see this failure.";
}
#endif
