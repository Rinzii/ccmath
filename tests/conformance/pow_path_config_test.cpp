#include "utils/pow_path_dispatch.hpp"

#include <gtest/gtest.h>

#include <array>

namespace
{
	template <typename T>
	void SmokePath(ccm::test::pow_path::validation_path path)
	{
		const auto support = ccm::test::pow_path::path_is_supported<T>(path);
		if (!support.supported) { GTEST_SKIP() << support.skip_reason; }

		const auto report = ccm::test::pow_path::make_configuration_report<T>(path);
		ccm::test::pow_path::print_configuration_banner(report);

		const std::array<T, 3> bases	 = { static_cast<T>(2), static_cast<T>(0.5), static_cast<T>(1.25) };
		const std::array<T, 3> exponents = { static_cast<T>(3), static_cast<T>(-2), static_cast<T>(0.5) };

		for (T base : bases)
		{
			for (T exponent : exponents)
			{
				if (path == ccm::test::pow_path::validation_path::generic_modeled_domain && !(base > T{})) { continue; }
				const T result = ccm::test::pow_path::invoke(path, base, exponent);
				EXPECT_TRUE(std::isfinite(result) || std::isinf(result) || std::isnan(result));
			}
		}
	}
} // namespace

TEST(PowPathConfig, PublicDefaultDouble)
{ SmokePath<double>(ccm::test::pow_path::validation_path::public_default); }

TEST(PowPathConfig, GenericRuntimeDouble)
{ SmokePath<double>(ccm::test::pow_path::validation_path::generic_runtime); }

TEST(PowPathConfig, GenericModeledDomainDouble)
{ SmokePath<double>(ccm::test::pow_path::validation_path::generic_modeled_domain); }

TEST(PowPathConfig, RuntimeNoBuiltinDouble)
{ SmokePath<double>(ccm::test::pow_path::validation_path::runtime_no_builtin); }

TEST(PowPathConfig, RuntimeSimdDouble)
{ SmokePath<double>(ccm::test::pow_path::validation_path::runtime_simd); }

TEST(PowPathConfig, RuntimeBuiltinDouble)
{ SmokePath<double>(ccm::test::pow_path::validation_path::runtime_builtin); }

TEST(PowPathConfig, PublicDefaultFloat)
{ SmokePath<float>(ccm::test::pow_path::validation_path::public_default); }

TEST(PowPathConfig, GenericRuntimeFloat)
{ SmokePath<float>(ccm::test::pow_path::validation_path::generic_runtime); }

TEST(PowPathConfig, RuntimeNoBuiltinFloat)
{ SmokePath<float>(ccm::test::pow_path::validation_path::runtime_no_builtin); }

TEST(PowPathConfig, RuntimeSimdFloat)
{ SmokePath<float>(ccm::test::pow_path::validation_path::runtime_simd); }

TEST(PowPathConfig, RuntimeBuiltinFloat)
{ SmokePath<float>(ccm::test::pow_path::validation_path::runtime_builtin); }
