#include <gtest/gtest.h>
#include <cmath>
#include <limits>
#include <ostream>
#include <type_traits>
#include "ccmath/internal/support/fp/fp_bits.hpp"

template <typename T>
int64_t ulp_difference(T a, T b)
{
	static_assert(std::is_floating_point_v<T>, "T must be a floating-point type.");

	using namespace ccm::support::fp;
	using FPBits_t = FPBits<T>;

	FPBits_t fp_a(a);
	FPBits_t fp_b(b);

	// Handle special cases like NaN or Infinity
	if (fp_a.is_nan() && fp_b.is_nan())
	{
		return 0; // Both are NaN, considered equal
	}
	if (fp_a.is_nan() || fp_b.is_nan())
	{
		return std::numeric_limits<int64_t>::max(); // One is NaN, the other is not
	}
	if (fp_a.is_inf() && fp_b.is_inf())
	{
		if (fp_a.sign() == fp_b.sign())
		{
			return 0; // Both infinities with the same sign are considered equal
		}
		return std::numeric_limits<int64_t>::max(); // Opposite sign infinities
	}
	if (fp_a.is_inf() || fp_b.is_inf())
	{
		return std::numeric_limits<int64_t>::max(); // One is infinity, the other is not
	}

	using UIntType = typename FPBits_t::storage_type;
	constexpr UIntType sign_mask = UIntType{ 1 } << (sizeof(UIntType) * 8 - 1);

	auto ordered_bits = [=](FPBits_t bits) {
		const UIntType raw = bits.uintval();
		if ((raw & sign_mask) != 0) { return sign_mask - (raw & ~sign_mask); }
		return sign_mask + raw;
	};

	UIntType const a_bits = ordered_bits(fp_a);
	UIntType const b_bits = ordered_bits(fp_b);
	UIntType const diff = (a_bits > b_bits) ? a_bits - b_bits : b_bits - a_bits;
	if (diff > static_cast<UIntType>(std::numeric_limits<int64_t>::max())) { return std::numeric_limits<int64_t>::max(); }
	return static_cast<int64_t>(diff);
}

template <typename T>
class ULPEqualsMatcher : public ::testing::MatcherInterface<T>
{
public:
	ULPEqualsMatcher(T expected, int64_t max_ulp) : expected_(expected), max_ulp_(max_ulp)
	{
		static_assert(std::is_floating_point_v<T>, "Expected must be a floating-point type.");
		if (max_ulp_ < 0) { throw std::invalid_argument("Maximum allowed ULPs must be non-negative."); }
	}

	bool MatchAndExplain(T actual, ::testing::MatchResultListener * listener) const override
	{
		const int64_t ulp_diff = ulp_difference(actual, expected_);
		if (ulp_diff <= max_ulp_) { return true; }
		if (listener)
		{
			*listener << "ULP Difference Exceeded:\n"
					  << "  Actual: " << actual << "\n"
					  << "  Expected: " << expected_ << "\n"
					  << "  ULP Difference: " << ulp_diff << "\n"
					  << "  Maximum Allowed ULPs: " << max_ulp_;
		}
		return false;
	}

	void DescribeTo(std::ostream * os) const override { *os << "is within " << max_ulp_ << " ULPs of " << expected_; }

	void DescribeNegationTo(std::ostream * os) const override { *os << "is not within " << max_ulp_ << " ULPs of " << expected_; }

private:
	T expected_;
	int64_t max_ulp_;
};

template <typename T>
::testing::Matcher<T> IsWithinULP(T expected, int64_t max_ulp)
{
	return ::testing::MakeMatcher(new ULPEqualsMatcher<T>(expected, max_ulp));
}

// Convenience Matcher for IsWithinULP with a maximum ULP difference of 1.
template <typename T>
::testing::Matcher<T> IsWithinOneULP(T expected)
{
	return IsWithinULP(expected, 1);
}

// EXPECT_ULP_NEAR checks if the ACTUAL value is within a specified maximum ULP difference from the EXPECTED value.
#define EXPECT_ULP_NEAR(ACTUAL, EXPECTED, MAX_ULP) EXPECT_THAT(ACTUAL, IsWithinULP(EXPECTED, MAX_ULP))

// EXPECT_ULP_EQ checks if the ACTUAL value is within one ULP difference from the EXPECTED value.
#define EXPECT_ULP_EQ(ACTUAL, EXPECTED) EXPECT_THAT(ACTUAL, IsWithinOneULP(EXPECTED))

// ASSERT_ULP_NEAR checks if the ACTUAL value is within a specified maximum ULP difference from the EXPECTED value and aborts the test if it fails.
#define ASSERT_ULP_NEAR(ACTUAL, EXPECTED, MAX_ULP) ASSERT_THAT(ACTUAL, IsWithinULP(EXPECTED, MAX_ULP))

// ASSERT_ULP_EQ checks if the ACTUAL value is within one ULP difference from the EXPECTED value and aborts the test if it fails.
#define ASSERT_ULP_EQ(ACTUAL, EXPECTED) ASSERT_THAT(ACTUAL, IsWithinOneULP(EXPECTED))