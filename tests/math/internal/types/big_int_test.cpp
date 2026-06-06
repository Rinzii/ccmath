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

#include "ccmath/internal/types/big_int.hpp"

namespace
{
	using UInt64 = ccm::types::UInt<64>;
	using UInt32 = ccm::types::UInt<32>;
} // namespace

TEST(CcmathInternalTypesTests, BigIntGetBitAndDecrement)
{
	UInt64 value(0xAULL);
	EXPECT_FALSE(value.get_bit(0));
	EXPECT_TRUE(value.get_bit(1));
	EXPECT_TRUE(value.get_bit(3));

	value = UInt64(1);
	--value;
	EXPECT_TRUE(value.is_zero());

	value = UInt64(0);
	--value;
	EXPECT_TRUE(value.get_bit(0));
	EXPECT_TRUE(value.get_bit(63));
}

TEST(CcmathInternalTypesTests, BigIntCrossWordConversion)
{
	UInt32 small(0x12345678U);
	const ccm::types::UInt<128> wide(small);

	EXPECT_EQ(wide[0], 0x12345678U);
	EXPECT_EQ(wide[1], 0U);

	const UInt32 narrowed(wide);
	EXPECT_EQ(static_cast<std::uint32_t>(narrowed), 0x12345678U);
}
