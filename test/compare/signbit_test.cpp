/*
* Copyright (c) 2024-Present Ian Pike
* Copyright (c) 2024-Present ccmath contributors
*
* This library is provided under the MIT License.
* See LICENSE for more information.
*/

#include <gtest/gtest.h>
#include <cmath>
#include <ccmath/detail/compare/signbit.hpp>
#include <limits>


TEST(CcmathCompareTests, Signbit)
{
	// Test that signbit is static_assert-able
    static_assert(ccm::signbit(1.0) == false, "signbit has failed testing that it is static_assert-able!");

	// MSVC has issues when std::signbit is called using an integer due to ambiguity
	EXPECT_EQ(ccm::signbit<float>(1.0), std::signbit(1.0));
    EXPECT_EQ(ccm::signbit<float>(-1.0), std::signbit(-1.0));
    EXPECT_EQ(ccm::signbit<float>(0.0), std::signbit(0.0));
    EXPECT_EQ(ccm::signbit<float>(-0.0), std::signbit(-0.0));
    EXPECT_EQ(ccm::signbit<float>(std::numeric_limits<double>::infinity()), std::signbit(std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::signbit<float>(-std::numeric_limits<double>::infinity()), std::signbit(-std::numeric_limits<double>::infinity()));
	EXPECT_EQ(ccm::signbit<float>(std::numeric_limits<double>::quiet_NaN()), std::signbit(std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(ccm::signbit<float>(-std::numeric_limits<double>::quiet_NaN()), std::signbit(-std::numeric_limits<double>::quiet_NaN()));







}
