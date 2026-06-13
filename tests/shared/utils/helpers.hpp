#include "utils/ulp_distance.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <ostream>
#include <type_traits>

template <typename T>
int64_t ulp_difference(T a, T b)
{
	static_assert(std::is_floating_point_v<T>, "T must be a floating-point type.");

	const std::uint64_t distance = ccm::test::ulp::distance_or_max(a, b);
	return (distance > static_cast<std::uint64_t>(std::numeric_limits<int64_t>::max())) ? std::numeric_limits<int64_t>::max() : static_cast<int64_t>(distance);
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
{ return ::testing::MakeMatcher(new ULPEqualsMatcher<T>(expected, max_ulp)); }

// Convenience Matcher for IsWithinULP with a maximum ULP difference of 1.
template <typename T>
::testing::Matcher<T> IsWithinOneULP(T expected)
{ return IsWithinULP(expected, 1); }

// EXPECT_ULP_NEAR checks if the ACTUAL value is within a specified maximum ULP difference from the EXPECTED value.
#define EXPECT_ULP_NEAR(ACTUAL, EXPECTED, MAX_ULP) EXPECT_THAT(ACTUAL, IsWithinULP(EXPECTED, MAX_ULP))

// EXPECT_ULP_EQ checks if the ACTUAL value is within one ULP difference from the EXPECTED value.
#define EXPECT_ULP_EQ(ACTUAL, EXPECTED) EXPECT_THAT(ACTUAL, IsWithinOneULP(EXPECTED))

// ASSERT_ULP_NEAR checks if the ACTUAL value is within a specified maximum ULP difference from the EXPECTED value and aborts the test if it fails.
#define ASSERT_ULP_NEAR(ACTUAL, EXPECTED, MAX_ULP) ASSERT_THAT(ACTUAL, IsWithinULP(EXPECTED, MAX_ULP))

// ASSERT_ULP_EQ checks if the ACTUAL value is within one ULP difference from the EXPECTED value and aborts the test if it fails.
#define ASSERT_ULP_EQ(ACTUAL, EXPECTED) ASSERT_THAT(ACTUAL, IsWithinOneULP(EXPECTED))
