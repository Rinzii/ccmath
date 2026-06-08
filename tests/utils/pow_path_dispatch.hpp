#pragma once

#include "ccmath/ccmath.hpp"
#include "ccmath/internal/math/generic/builtins/power/pow.hpp"
#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"
#include "ccmath/internal/math/runtime/func/power/pow_rt.hpp"
#include "utils/campaign_metadata.hpp"
#include "utils/test_runtime.hpp"

#include <cmath>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

namespace ccm::test::pow_path
{
	enum class validation_path
	{
		public_default,
		generic_runtime,
		generic_modeled_domain,
		runtime_no_builtin,
		runtime_simd,
		runtime_builtin,
	};

	inline std::string path_name(validation_path path)
	{
		switch (path)
		{
		case validation_path::public_default: return "public_default";
		case validation_path::generic_runtime: return "generic_runtime";
		case validation_path::generic_modeled_domain: return "generic_modeled_domain";
		case validation_path::runtime_no_builtin: return "runtime_no_builtin";
		case validation_path::runtime_simd: return "runtime_simd";
		case validation_path::runtime_builtin: return "runtime_builtin";
		}
		return "unknown";
	}

	inline std::optional<validation_path> parse_path(std::string_view raw)
	{
		if (raw == "public" || raw == "public_default" || raw == "public_api_runtime") { return validation_path::public_default; }
		if (raw == "generic" || raw == "generic_runtime" || raw == "generic_pow_gen") { return validation_path::generic_runtime; }
		if (raw == "generic_modeled_domain" || raw == "generic_pow_gen_modeled_domain") { return validation_path::generic_modeled_domain; }
		if (raw == "runtime_no_builtin") { return validation_path::runtime_no_builtin; }
		if (raw == "runtime_simd") { return validation_path::runtime_simd; }
		if (raw == "runtime_builtin") { return validation_path::runtime_builtin; }
		return std::nullopt;
	}

	struct path_support
	{
		bool supported = false;
		std::string skip_reason;
	};

	template <typename T>
	inline path_support path_is_supported(validation_path path)
	{
		switch (path)
		{
		case validation_path::public_default:
		case validation_path::generic_runtime:
		case validation_path::generic_modeled_domain: return { true, {} };
		case validation_path::runtime_no_builtin:
#if defined(CCM_CONFIG_TEST_DISABLE_RUNTIME_BUILTIN_POW)
			return { true, {} };
#else
			return { false, "runtime_no_builtin requires CCMATH_TEST_DISABLE_RUNTIME_BUILTIN=ON at configure time" };
#endif
		case validation_path::runtime_simd:
#if defined(CCMATH_HAS_SIMD)
			if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) { return { true, {} }; }
			return { false, "runtime_simd is only supported for float and double" };
#else
			return { false, "runtime_simd requires runtime SIMD to be compiled in" };
#endif
		case validation_path::runtime_builtin:
			if constexpr (ccm::builtin::has_runtime_pow<T>) { return { true, {} }; }
			return { false, "runtime_builtin is unavailable on this toolchain" };
		}
		return { false, "unknown path" };
	}

	template <typename T>
	inline T invoke(validation_path path, T base, T exponent)
	{
		switch (path)
		{
		case validation_path::public_default:
			if constexpr (std::is_same_v<T, float>) { return ccm::powf(ccm::test::runtime_value(base), ccm::test::runtime_value(exponent)); }
			else if constexpr (std::is_same_v<T, double>) { return ccm::pow(ccm::test::runtime_value(base), ccm::test::runtime_value(exponent)); }
			else
			{
				return ccm::powl(ccm::test::runtime_value(base), ccm::test::runtime_value(exponent));
			}
		case validation_path::generic_runtime:
		case validation_path::generic_modeled_domain: return ccm::gen::pow_gen(ccm::test::runtime_value(base), ccm::test::runtime_value(exponent));
		case validation_path::runtime_no_builtin: return ccm::rt::pow_rt(ccm::test::runtime_value(base), ccm::test::runtime_value(exponent));
		case validation_path::runtime_simd:
#if defined(CCMATH_HAS_SIMD)
			return ccm::rt::simd_impl::pow_simd_impl(ccm::test::runtime_value(base), ccm::test::runtime_value(exponent));
#else
			return ccm::gen::pow_gen(ccm::test::runtime_value(base), ccm::test::runtime_value(exponent));
#endif
		case validation_path::runtime_builtin:
			// runtime_pow is only defined where has_runtime_pow<T> holds (GCC/Clang). On other
			// toolchains this path reports unsupported via path_is_supported, but the switch must
			// still compile, so fall back to the generic kernel in the discarded branch.
			if constexpr (ccm::builtin::has_runtime_pow<T>)
			{
				return ccm::builtin::runtime_pow(ccm::test::runtime_value(base), ccm::test::runtime_value(exponent));
			}
			else
			{
				return ccm::gen::pow_gen(ccm::test::runtime_value(base), ccm::test::runtime_value(exponent));
			}
		}
		return ccm::gen::pow_gen(ccm::test::runtime_value(base), ccm::test::runtime_value(exponent));
	}

	struct configuration_report
	{
		std::string configuration_name;
		std::string path;
		std::string compiler;
		std::string platform;
		std::string optimization_mode;
		std::string fma_enabled;
		std::string builtin_available;
		std::string simd_available;
	};

	template <typename T>
	inline configuration_report make_configuration_report(validation_path path)
	{
		return configuration_report{
			ccm::test::pow_configuration_name(), path_name(path),		  ccm::test::compiler_id(),			  ccm::test::platform_id(),
			ccm::test::optimization_mode(),		 ccm::test::fma_status(), ccm::test::pow_builtin_status<T>(), ccm::test::simd_status(),
		};
	}

	inline std::string configuration_report_json(const configuration_report & report)
	{
		std::ostringstream oss;
		oss << "{"
			<< "\"configuration_name\": \"" << report.configuration_name << "\","
			<< "\"path\": \"" << report.path << "\","
			<< "\"compiler\": \"" << report.compiler << "\","
			<< "\"platform\": \"" << report.platform << "\","
			<< "\"optimization_mode\": \"" << report.optimization_mode << "\","
			<< "\"fma_enabled\": \"" << report.fma_enabled << "\","
			<< "\"builtin_available\": \"" << report.builtin_available << "\","
			<< "\"simd_available\": \"" << report.simd_available << "\""
			<< "}";
		return oss.str();
	}

	inline void print_configuration_banner(const configuration_report & report)
	{ std::cout << "pow validation configuration: " << configuration_report_json(report) << '\n'; }

} // namespace ccm::test::pow_path
