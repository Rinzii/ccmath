/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "ccmath/internal/support/math_support.hpp"

#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <cstdint>
#include <limits>

namespace
{
	template <typename T>
	[[nodiscard]] constexpr T trailing_ones_ref(std::size_t count)
	{
		constexpr std::size_t digits = std::numeric_limits<T>::digits;
		if (count == 0) { return T(0); }
		if (count == digits) { return std::numeric_limits<T>::max(); }
		return static_cast<T>((T(1) << count) - T(1));
	}

	template <typename T>
	[[nodiscard]] constexpr T leading_ones_ref(std::size_t count)
	{
		constexpr std::size_t digits = std::numeric_limits<T>::digits;
		if (count == 0) { return T(0); }
		if (count == digits) { return std::numeric_limits<T>::max(); }
		return static_cast<T>(~trailing_ones_ref<T>(digits - count));
	}

	template <typename T, std::size_t Count = 0>
	void expect_masks_for_all_counts()
	{
		EXPECT_EQ((ccm::support::mask_trailing_ones<T, Count>()), trailing_ones_ref<T>(Count));
		EXPECT_EQ((ccm::support::mask_leading_ones<T, Count>()), leading_ones_ref<T>(Count));

		if constexpr (Count < std::numeric_limits<T>::digits) { expect_masks_for_all_counts<T, Count + 1>(); }
	}
} // namespace

TEST(CcmathInternalSupportTests, AddOverflowUint8MatchesReferenceExhaustively)
{
	for (int lhs = 0; lhs <= 0xFF; ++lhs)
	{
		for (int rhs = 0; rhs <= 0xFF; ++rhs)
		{
			std::uint8_t out			 = 0;
			const bool overflow			 = ccm::support::add_overflow<std::uint8_t>(static_cast<std::uint8_t>(lhs), static_cast<std::uint8_t>(rhs), out);
			const std::uint16_t expected = static_cast<std::uint16_t>(lhs + rhs);

			EXPECT_EQ(out, static_cast<std::uint8_t>(expected));
			EXPECT_EQ(overflow, expected > std::numeric_limits<std::uint8_t>::max());
		}
	}
}

TEST(CcmathInternalSupportTests, SubOverflowUint8MatchesReferenceExhaustively)
{
	for (int lhs = 0; lhs <= 0xFF; ++lhs)
	{
		for (int rhs = 0; rhs <= 0xFF; ++rhs)
		{
			std::uint8_t out			 = 0;
			const bool overflow			 = ccm::support::sub_overflow<std::uint8_t>(static_cast<std::uint8_t>(lhs), static_cast<std::uint8_t>(rhs), out);
			const bool expected_overflow = lhs < rhs;

			EXPECT_EQ(out, static_cast<std::uint8_t>(lhs - rhs));
			EXPECT_EQ(overflow, expected_overflow);
		}
	}
}

TEST(CcmathInternalSupportTests, AddOverflowInt8MatchesReferenceExhaustively)
{
	for (int lhs = std::numeric_limits<std::int8_t>::min(); lhs <= std::numeric_limits<std::int8_t>::max(); ++lhs)
	{
		for (int rhs = std::numeric_limits<std::int8_t>::min(); rhs <= std::numeric_limits<std::int8_t>::max(); ++rhs)
		{
			std::int8_t out				 = 0;
			const bool overflow			 = ccm::support::add_overflow<std::int8_t>(static_cast<std::int8_t>(lhs), static_cast<std::int8_t>(rhs), out);
			const int expected			 = lhs + rhs;
			const bool expected_overflow = expected < std::numeric_limits<std::int8_t>::min() || expected > std::numeric_limits<std::int8_t>::max();

			EXPECT_EQ(overflow, expected_overflow);
			if (!expected_overflow) { EXPECT_EQ(out, static_cast<std::int8_t>(expected)); }
		}
	}
}

TEST(CcmathInternalSupportTests, SubOverflowInt8MatchesReferenceExhaustively)
{
	for (int lhs = std::numeric_limits<std::int8_t>::min(); lhs <= std::numeric_limits<std::int8_t>::max(); ++lhs)
	{
		for (int rhs = std::numeric_limits<std::int8_t>::min(); rhs <= std::numeric_limits<std::int8_t>::max(); ++rhs)
		{
			std::int8_t out				 = 0;
			const bool overflow			 = ccm::support::sub_overflow<std::int8_t>(static_cast<std::int8_t>(lhs), static_cast<std::int8_t>(rhs), out);
			const int expected			 = lhs - rhs;
			const bool expected_overflow = expected < std::numeric_limits<std::int8_t>::min() || expected > std::numeric_limits<std::int8_t>::max();

			EXPECT_EQ(overflow, expected_overflow);
			if (!expected_overflow) { EXPECT_EQ(out, static_cast<std::int8_t>(expected)); }
		}
	}
}

TEST(CcmathInternalSupportTests, AddWithCarryUint8MatchesReferenceExhaustively)
{
	for (int lhs = 0; lhs <= 0xFF; ++lhs)
	{
		for (int rhs = 0; rhs <= 0xFF; ++rhs)
		{
			for (std::uint8_t carry_in : { std::uint8_t(0), std::uint8_t(1) })
			{
				std::uint8_t carry_out = 0;
				const std::uint8_t actual =
					ccm::support::add_with_carry<std::uint8_t>(static_cast<std::uint8_t>(lhs), static_cast<std::uint8_t>(rhs), carry_in, carry_out);

				const std::uint16_t expected = static_cast<std::uint16_t>(lhs) + static_cast<std::uint16_t>(rhs) + carry_in;
				EXPECT_EQ(actual, static_cast<std::uint8_t>(expected));
				EXPECT_EQ(carry_out, expected > std::numeric_limits<std::uint8_t>::max() ? 1U : 0U);
			}
		}
	}
}

TEST(CcmathInternalSupportTests, SubWithBorrowUint8MatchesReferenceExhaustively)
{
	for (int lhs = 0; lhs <= 0xFF; ++lhs)
	{
		for (int rhs = 0; rhs <= 0xFF; ++rhs)
		{
			for (std::uint8_t borrow_in : { std::uint8_t(0), std::uint8_t(1) })
			{
				std::uint8_t borrow_out = 0;
				const std::uint8_t actual =
					ccm::support::sub_with_borrow<std::uint8_t>(static_cast<std::uint8_t>(lhs), static_cast<std::uint8_t>(rhs), borrow_in, borrow_out);

				const int expected = lhs - rhs - borrow_in;
				EXPECT_EQ(actual, static_cast<std::uint8_t>(expected));
				EXPECT_EQ(borrow_out, expected < 0 ? 1U : 0U);
			}
		}
	}
}

TEST(CcmathInternalSupportTests, AddWithCarryUint64MatchesBoundaryCases)
{
	struct case_data
	{
		std::uint64_t lhs;
		std::uint64_t rhs;
		std::uint64_t carry_in;
		std::uint64_t expected_sum;
		std::uint64_t expected_carry;
	};

	const std::array<case_data, 8> cases = { {
		{ 0ULL, 0ULL, 0ULL, 0ULL, 0ULL },
		{ 0ULL, 0ULL, 1ULL, 1ULL, 0ULL },
		{ std::numeric_limits<std::uint64_t>::max(), 0ULL, 1ULL, 0ULL, 1ULL },
		{ std::numeric_limits<std::uint64_t>::max(), 1ULL, 0ULL, 0ULL, 1ULL },
		{ std::numeric_limits<std::uint64_t>::max(), std::numeric_limits<std::uint64_t>::max(), 0ULL, std::numeric_limits<std::uint64_t>::max() - 1ULL, 1ULL },
		{ std::numeric_limits<std::uint64_t>::max() - 1ULL, 1ULL, 1ULL, 0ULL, 1ULL },
		{ 0xFFFFFFFF00000000ULL, 0x00000000FFFFFFFFULL, 1ULL, 0ULL, 1ULL },
		{ 0x8000000000000000ULL, 0x8000000000000000ULL, 0ULL, 0ULL, 1ULL },
	} };

	for (const auto& test_case : cases)
	{
		std::uint64_t carry_out	   = 0;
		const std::uint64_t actual = ccm::support::add_with_carry<std::uint64_t>(test_case.lhs, test_case.rhs, test_case.carry_in, carry_out);

		EXPECT_EQ(actual, test_case.expected_sum);
		EXPECT_EQ(carry_out, test_case.expected_carry);
	}
}

TEST(CcmathInternalSupportTests, SubWithBorrowUint64MatchesBoundaryCases)
{
	struct case_data
	{
		std::uint64_t lhs;
		std::uint64_t rhs;
		std::uint64_t borrow_in;
		std::uint64_t expected_diff;
		std::uint64_t expected_borrow;
	};

	const std::array<case_data, 8> cases = { {
		{ 0ULL, 0ULL, 0ULL, 0ULL, 0ULL },
		{ 0ULL, 0ULL, 1ULL, std::numeric_limits<std::uint64_t>::max(), 1ULL },
		{ 0ULL, 1ULL, 0ULL, std::numeric_limits<std::uint64_t>::max(), 1ULL },
		{ 5ULL, 3ULL, 1ULL, 1ULL, 0ULL },
		{ std::numeric_limits<std::uint64_t>::max(), std::numeric_limits<std::uint64_t>::max(), 1ULL, std::numeric_limits<std::uint64_t>::max(), 1ULL },
		{ 0x100000000ULL, 1ULL, 1ULL, 0xFFFFFFFEULL, 0ULL },
		{ 0x8000000000000000ULL, 0x7FFFFFFFFFFFFFFFULL, 1ULL, 0ULL, 0ULL },
		{ 0x7FFFFFFFFFFFFFFFULL, 0x8000000000000000ULL, 0ULL, std::numeric_limits<std::uint64_t>::max(), 1ULL },
	} };

	for (const auto& test_case : cases)
	{
		std::uint64_t borrow_out   = 0;
		const std::uint64_t actual = ccm::support::sub_with_borrow<std::uint64_t>(test_case.lhs, test_case.rhs, test_case.borrow_in, borrow_out);

		EXPECT_EQ(actual, test_case.expected_diff);
		EXPECT_EQ(borrow_out, test_case.expected_borrow);
	}
}

TEST(CcmathInternalSupportTests, MaskHelpersMatchReferenceForAllCounts)
{
	expect_masks_for_all_counts<std::uint8_t>();
	expect_masks_for_all_counts<std::uint16_t>();
	expect_masks_for_all_counts<std::uint32_t>();
	expect_masks_for_all_counts<std::uint64_t>();
}

TEST(CcmathInternalSupportTests, FastTwoSumProducesKnownResiduals)
{
	double hi = 0.0;
	double lo = 0.0;

	ccm::support::fast_two_sum(hi, lo, 1.0, 0x1p-54);
	EXPECT_DOUBLE_EQ(hi, 1.0);
	EXPECT_DOUBLE_EQ(lo, 0x1p-54);

	ccm::support::fast_two_sum(hi, lo, -1.0, -0x1p-54);
	EXPECT_DOUBLE_EQ(hi, -1.0);
	EXPECT_DOUBLE_EQ(lo, -0x1p-54);

	ccm::support::fast_two_sum(hi, lo, 0x1.fffffffffffffp+52, 1.0);
	EXPECT_DOUBLE_EQ(hi, 0x1p+53);
	EXPECT_DOUBLE_EQ(lo, 0.0);
}

TEST(CcmathInternalSupportTests, TwoSumProducesKnownResiduals)
{
	double sum = 0.0;
	double err = 0.0;

	ccm::support::two_sum(sum, err, 0x1p-54, 1.0);
	EXPECT_DOUBLE_EQ(sum, 1.0);
	EXPECT_DOUBLE_EQ(err, 0x1p-54);

	ccm::support::two_sum(sum, err, -0x1p-54, -1.0);
	EXPECT_DOUBLE_EQ(sum, -1.0);
	EXPECT_DOUBLE_EQ(err, -0x1p-54);

	ccm::support::two_sum(sum, err, 0x1.fffffffffffffp+52, 1.0);
	EXPECT_DOUBLE_EQ(sum, 0x1p+53);
	EXPECT_DOUBLE_EQ(err, 0.0);
}

TEST(CcmathInternalSupportTests, VeltkampSplitReconstructsInput)
{
	long double xh = 0.0L;
	long double xl = 0.0L;

	for (long double input : { 0.0L, 1.0L, -1.0L, 0x1.12345678p+40L, -0x1.98765432p-20L, 0x1.ffffffffp+12L })
	{
		ccm::support::veltkamp_split(xh, xl, input);
		EXPECT_EQ(xh + xl, input);
		EXPECT_LE(std::abs(xl), std::abs(input) + 1.0L);
	}
}
