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

#include "ccmath/ccmath.hpp"

#include <cmath>
#include <limits>

namespace {
	// Equivelant to EXPECT_EQ but allows for NaN values to be equal.
	bool IsNanOrEquivalent(double a, double b) {
		return (std::isnan(a) && std::isnan(b)) || (a == b);
	}
}

/*
TEST(CcmathPowerTests, Pow_Double_SpecialCases)
{
	EXPECT_EQ(ccm::internal::impl::pow_impl(std::numeric_limits<double>::quiet_NaN(), 0.0), std::pow(std::numeric_limits<double>::quiet_NaN(), 0.0));

	bool ccm_is_signaling_nan_x_and_y_zero_nan = std::isnan(ccm::internal::impl::pow_impl(std::numeric_limits<double>::signaling_NaN(), 0.0));
	bool std_is_signaling_nan_x_and_y_zero_nan = std::isnan(std::pow(std::numeric_limits<double>::signaling_NaN(), 0.0));
	EXPECT_EQ(ccm_is_signaling_nan_x_and_y_zero_nan, std_is_signaling_nan_x_and_y_zero_nan);

	// When handling signaling NaN, the result is supposed to be NaN as dictated by IEEE-754 2019, but
	// GCC based compilers are not conforming in this reguard so we allow for NaN or equal values.
	// Also Clang will change its return type based on the optimization level.
	// Expected values here would be either NaN or 1.0
	auto ccm_x_snan_y_zero = ccm::internal::impl::pow_impl(std::numeric_limits<double>::signaling_NaN(), 0.0);
	auto std_x_snan_y_zero = std::pow(std::numeric_limits<double>::signaling_NaN(), 0.0);
	//EXPECT_EQ(ccm_x_snan_y_zero, std_x_snan_y_zero);
	EXPECT_PRED2(IsNanOrEquivalent, ccm_x_snan_y_zero, std_x_snan_y_zero);

	bool ccm_is_nan_x_and_y_one_nan = std::isnan(ccm::internal::impl::pow_impl(std::numeric_limits<double>::quiet_NaN(), 1.0));
	bool std_is_nan_x_and_y_one_nan = std::isnan(std::pow(std::numeric_limits<double>::quiet_NaN(), 1.0));
	EXPECT_EQ(ccm_is_nan_x_and_y_one_nan, std_is_nan_x_and_y_one_nan);

	EXPECT_EQ(ccm::internal::impl::pow_impl(1.0, std::numeric_limits<double>::quiet_NaN()), std::pow(1.0, std::numeric_limits<double>::quiet_NaN()));

	bool ccm_is_one_x_and_signaling_nan_y_nan = std::isnan(ccm::internal::impl::pow_impl(1.0, std::numeric_limits<double>::signaling_NaN()));
	bool std_is_one_x_and_signaling_nan_y_nan = std::isnan(std::pow(1.0, std::numeric_limits<double>::signaling_NaN()));
	EXPECT_EQ(ccm_is_one_x_and_signaling_nan_y_nan, std_is_one_x_and_signaling_nan_y_nan);

	bool ccm_is_one_x_and_nan_y_nan = std::isnan(ccm::internal::impl::pow_impl(1.0, std::numeric_limits<double>::quiet_NaN()));
	bool std_is_one_x_and_nan_y_nan = std::isnan(std::pow(1.0, std::numeric_limits<double>::quiet_NaN()));
	EXPECT_EQ(ccm_is_one_x_and_nan_y_nan, std_is_one_x_and_nan_y_nan);


	/// Check for when x is positive infinity

	EXPECT_EQ(ccm::internal::impl::pow_impl(std::numeric_limits<double>::infinity(), 0.0), std::pow(std::numeric_limits<double>::infinity(), 0.0));

	EXPECT_EQ(ccm::internal::impl::pow_impl(std::numeric_limits<double>::infinity(), -1.0), std::pow(std::numeric_limits<double>::infinity(), -1.0));
	EXPECT_EQ(ccm::internal::impl::pow_impl(std::numeric_limits<double>::infinity(), 1.0), std::pow(std::numeric_limits<double>::infinity(), 1.0));


	// Check for when x is negative infinity
	EXPECT_EQ(ccm::internal::impl::pow_impl(-std::numeric_limits<double>::infinity(), 3.0), std::pow(-std::numeric_limits<double>::infinity(), 3.0));


}
*/
/*
TEST(CcmathPowerTests, Pow_Unsigned_Integral)
{
	constexpr unsigned int x = 2;
	constexpr unsigned int y = 2;
	EXPECT_EQ(ccm::internal::impl::pow_expo_by_sqr(x, y), std::pow(x, y));
}

TEST(CcmathPowerTests, Pow_Double)
{
	static_assert(ccm::pow(2.0, 2.0) == 4, "ccm::pow is not working with static_assert!");

	EXPECT_EQ(std::pow(14.0, 2.0), std::exp2(2.0 * std::log2(14.0)));


	// Test for a base of 2
	EXPECT_EQ(ccm::pow(2.0, 0.5), std::pow(2.0, 0.5));
	EXPECT_EQ(ccm::pow(2.0, 1.0), std::pow(2.0, 1.0));
	EXPECT_EQ(ccm::pow(2.0, 2.0), std::pow(2.0, 2.0));
	EXPECT_EQ(ccm::pow(2.0, 3.0), std::pow(2.0, 3.0));
	EXPECT_EQ(ccm::pow(2.0, 4.0), std::pow(2.0, 4.0));
	EXPECT_EQ(ccm::pow(2.0, 5.0), std::pow(2.0, 5.0));
	EXPECT_EQ(ccm::pow(2.0, 6.0), std::pow(2.0, 6.0));
	EXPECT_EQ(ccm::pow(2.0, 7.0), std::pow(2.0, 7.0));
	EXPECT_EQ(ccm::pow(2.0, 8.0), std::pow(2.0, 8.0));

	// Test for a even base that is not a multiple of 2^n
	EXPECT_EQ(ccm::pow(14.0, 0.5), std::pow(14.0, 0.5));
	EXPECT_EQ(ccm::pow(14.0, 1.0), std::pow(14.0, 1.0));
	EXPECT_EQ(ccm::pow(14.0, 2.0), std::pow(14.0, 2.0));
	EXPECT_EQ(ccm::pow(14.0, 3.0), std::pow(14.0, 3.0));
	EXPECT_EQ(ccm::pow(14.0, 4.0), std::pow(14.0, 4.0));
	EXPECT_EQ(ccm::pow(14.0, 5.0), std::pow(14.0, 5.0));
	EXPECT_EQ(ccm::pow(14.0, 6.0), std::pow(14.0, 6.0));
	EXPECT_EQ(ccm::pow(14.0, 7.0), std::pow(14.0, 7.0));
	EXPECT_EQ(ccm::pow(14.0, 8.0), std::pow(14.0, 8.0));

	// Test for an odd base
	EXPECT_EQ(ccm::pow(3.0, 0.5), std::pow(3.0, 0.5));
	EXPECT_EQ(ccm::pow(3.0, 1.0), std::pow(3.0, 1.0));
	EXPECT_EQ(ccm::pow(3.0, 2.0), std::pow(3.0, 2.0));
	EXPECT_EQ(ccm::pow(3.0, 3.0), std::pow(3.0, 3.0));
	EXPECT_EQ(ccm::pow(3.0, 4.0), std::pow(3.0, 4.0));
	EXPECT_EQ(ccm::pow(3.0, 5.0), std::pow(3.0, 5.0));
	EXPECT_EQ(ccm::pow(3.0, 6.0), std::pow(3.0, 6.0));
	EXPECT_EQ(ccm::pow(3.0, 7.0), std::pow(3.0, 7.0));
	EXPECT_EQ(ccm::pow(3.0, 8.0), std::pow(3.0, 8.0));

	// Test for negative base
	EXPECT_EQ(ccm::pow(-4.0, 0.5), std::pow(-4.0, 0.5));
	EXPECT_EQ(ccm::pow(-4.0, 1.0), std::pow(-4.0, 1.0));
	EXPECT_EQ(ccm::pow(-4.0, 2.0), std::pow(-4.0, 2.0));
	EXPECT_EQ(ccm::pow(-4.0, 3.0), std::pow(-4.0, 3.0));
	EXPECT_EQ(ccm::pow(-4.0, 4.0), std::pow(-4.0, 4.0));
	EXPECT_EQ(ccm::pow(-4.0, 5.0), std::pow(-4.0, 5.0));
	EXPECT_EQ(ccm::pow(-4.0, 6.0), std::pow(-4.0, 6.0));
	EXPECT_EQ(ccm::pow(-4.0, 7.0), std::pow(-4.0, 7.0));
	EXPECT_EQ(ccm::pow(-4.0, 8.0), std::pow(-4.0, 8.0));

	// Test for negative base and exponent
	EXPECT_EQ(ccm::pow(-4.0, -0.5), std::pow(-4.0, -0.5));
	EXPECT_EQ(ccm::pow(-4.0, -1.0), std::pow(-4.0, -1.0));
	EXPECT_EQ(ccm::pow(-4.0, -2.0), std::pow(-4.0, -2.0));
	EXPECT_EQ(ccm::pow(-4.0, -3.0), std::pow(-4.0, -3.0));
	EXPECT_EQ(ccm::pow(-4.0, -4.0), std::pow(-4.0, -4.0));
	EXPECT_EQ(ccm::pow(-4.0, -5.0), std::pow(-4.0, -5.0));
	EXPECT_EQ(ccm::pow(-4.0, -6.0), std::pow(-4.0, -6.0));
	EXPECT_EQ(ccm::pow(-4.0, -7.0), std::pow(-4.0, -7.0));
	EXPECT_EQ(ccm::pow(-4.0, -8.0), std::pow(-4.0, -8.0));

	// Test for positive base and negative exponent
	EXPECT_EQ(ccm::pow(20.0, -0.5), std::pow(20.0, -0.5));
	EXPECT_EQ(ccm::pow(20.0, -1.0), std::pow(20.0, -1.0));
	EXPECT_EQ(ccm::pow(20.0, -2.0), std::pow(20.0, -2.0));
	EXPECT_EQ(ccm::pow(20.0, -3.0), std::pow(20.0, -3.0));
	EXPECT_EQ(ccm::pow(20.0, -4.0), std::pow(20.0, -4.0));
	EXPECT_EQ(ccm::pow(20.0, -5.0), std::pow(20.0, -5.0));
	EXPECT_EQ(ccm::pow(20.0, -6.0), std::pow(20.0, -6.0));
	EXPECT_EQ(ccm::pow(20.0, -7.0), std::pow(20.0, -7.0));
	EXPECT_EQ(ccm::pow(20.0, -8.0), std::pow(20.0, -8.0));

	// Test for large base and small exponent
	EXPECT_EQ(ccm::pow(123456.7, 0.5), std::pow(123456.7, 0.5));
	EXPECT_EQ(ccm::pow(123456.7, 1.0), std::pow(123456.7, 1.0));
	EXPECT_EQ(ccm::pow(123456.7, 2.0), std::pow(123456.7, 2.0));
	EXPECT_EQ(ccm::pow(123456.7, 3.0), std::pow(123456.7, 3.0));
	EXPECT_EQ(ccm::pow(123456.7, 4.0), std::pow(123456.7, 4.0));
	EXPECT_EQ(ccm::pow(123456.7, 5.0), std::pow(123456.7, 5.0));
	EXPECT_EQ(ccm::pow(123456.7, 6.0), std::pow(123456.7, 6.0));
	EXPECT_EQ(ccm::pow(123456.7, 7.0), std::pow(123456.7, 7.0));
	EXPECT_EQ(ccm::pow(123456.7, 8.0), std::pow(123456.7, 8.0));

	// Test for small base and large exponent
	EXPECT_EQ(ccm::pow(0.1, 123456.7), std::pow(0.1, 123456.7));
	EXPECT_EQ(ccm::pow(0.2, 123456.7), std::pow(0.2, 123456.7));
	EXPECT_EQ(ccm::pow(0.3, 123456.7), std::pow(0.3, 123456.7));
	EXPECT_EQ(ccm::pow(0.4, 123456.7), std::pow(0.4, 123456.7));
	EXPECT_EQ(ccm::pow(0.5, 123456.7), std::pow(0.5, 123456.7));
	EXPECT_EQ(ccm::pow(0.6, 123456.7), std::pow(0.6, 123456.7));
	EXPECT_EQ(ccm::pow(0.7, 123456.7), std::pow(0.7, 123456.7));
	EXPECT_EQ(ccm::pow(0.8, 123456.7), std::pow(0.8, 123456.7));
}
 */
