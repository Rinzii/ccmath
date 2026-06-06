#include "utils/pow_path_dispatch.hpp"

#include "ccmath/internal/config/long_double_format.hpp"
#include "ccmath/internal/config/powl_policy.hpp"
#include "oracle/powl_path_reporting.hpp"

#include <cmath>
#include <gtest/gtest.h>
#include <limits>

TEST(PowlPlatform, CharacterizationSmoke)
{
	const std::size_t ld_size = sizeof(long double);
	const int digits = std::numeric_limits<long double>::digits;
	EXPECT_GT(ld_size, 0u);
	EXPECT_GT(digits, 0);

	const auto format = ccm::config::detect_long_double_format();
	std::cout << "powl platform smoke: sizeof=" << ld_size << " digits=" << digits << " format="
			  << ccm::config::long_double_format_name(format)
			  << " fallback=" << (ccm::config::reduced_precision_powl_fallback_enabled() ? "enabled" : "disabled")
			  << " configuration=" << ccm::test::pow_path::configuration_name() << '\n';

	const long double result = ccm::powl(2.0L, 3.0L);
	EXPECT_EQ(result, 8.0L);
}

TEST(PowlPlatform, StdPowRegressionCorpus)
{
	const std::array<long double, 4> bases = { 0.5L, 1.0L, 2.0L, 3.0L };
	const std::array<long double, 4> exponents = { -1.0L, 0.5L, 2.0L, 3.0L };
	for (long double base : bases)
	{
		for (long double exponent : exponents)
		{
			if (!ccm::config::reduced_precision_powl_fallback_enabled())
			{
				const auto path = ccm::test::oracle::classify_powl_gen_path(base, exponent);
				if (!ccm::test::oracle::powl_path_is_native_phase1(path)) { continue; }
			}

			const long double expected = std::pow(base, exponent);
			if (std::isnan(expected)) { continue; }
			EXPECT_EQ(ccm::powl(base, exponent), expected);
		}
	}
}
