#include "ccmath/internal/config/long_double_format.hpp"
#include "ccmath/internal/config/powl_policy.hpp"
#include "mpfr_pow_common.hpp"
#include "utils/pow_path_dispatch.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>

namespace
{
	struct platform_report
	{
		std::size_t sizeof_long_double				 = 0;
		int digits									 = 0;
		int max_exponent							 = 0;
		ccm::config::LongDoubleFormat classification = ccm::config::LongDoubleFormat::Unknown;
		std::string classification_name;
		bool fallback_enabled = false;
		std::string powl_path;
		bool mpfr_oracle_supported = false;
		std::string mpfr_skip_reason;
		std::size_t oracle_case_count = 0;
		std::uint64_t oracle_max_ulp  = 0;
	};

	// ReSharper disable once CppDFAConstantFunctionResult
	std::string detect_powl_path()
	{
#if defined(CCMATH_HAS_CONSTEXPR_BUILTIN_POW)
		if (ccm::builtin::has_constexpr_pow<long double>) { return "constexpr_builtin"; }
#endif
		if (ccm::builtin::has_runtime_pow<long double>) { return "runtime_builtin"; }
#if defined(CCMATH_HAS_SIMD)
		return "runtime_simd_or_generic";
#else
		return "generic_runtime";
#endif
	}

	void run_conservative_oracle(platform_report & report)
	{
		// ReSharper disable once CppDFAConstantConditions
		if (report.classification != ccm::config::LongDoubleFormat::Double)
		{
			// ReSharper disable CppDFAUnreachableCode
			report.mpfr_oracle_supported = false;
			report.mpfr_skip_reason		 = "MPFR conservative oracle uses double reference only on double-shaped long double platforms";
			return;
			// ReSharper enable CppDFAUnreachableCode
		}

		const std::array bases	   = { 0.25L, 0.5L, 1.0L, 2.0L, 3.0L, 10.0L };
		const std::array exponents = { -2.0L, -0.5L, 0.0L, 0.5L, 2.0L, 3.0L };
		ccm::test::oracle::run_summary<long double> summary;

		for (long double base : bases)
		{
			for (long double exponent : exponents)
			{
				const double base_d			  = static_cast<double>(base);
				const double exp_d			  = static_cast<double>(exponent);
				const long double actual	  = ccm::powl(base, exponent);
				const double expected		  = ccm::test::oracle::mpfr_pow_reference(base_d, exp_d, 256, ccm::test::oracle::current_mpfr_rounding_mode());
				const long double expected_ld = expected;

				std::uint64_t distance = 0;
				std::string notes;
				const bool pass = ccm::test::oracle::oracle_match(actual, expected_ld, 4, distance, notes);
				++summary.case_count;
				if (distance >= summary.max_observed_ulp)
				{
					summary.max_observed_ulp = distance;
					summary.worst_base		 = base;
					summary.worst_exponent	 = exponent;
					summary.worst_actual	 = actual;
					summary.worst_expected	 = expected_ld;
				}
				if (!pass) { ++summary.failure_count; }
			}
		}

		report.mpfr_oracle_supported = true;
		report.oracle_case_count	 = summary.case_count;
		report.oracle_max_ulp		 = summary.max_observed_ulp;
	}

	void write_json(const std::string & path, const platform_report & report)
	{
		std::ofstream out(path, std::ios::trunc);
		out << "{\n";
		out << "  \"sizeof_long_double\": " << report.sizeof_long_double << ",\n";
		out << "  \"digits\": " << report.digits << ",\n";
		out << "  \"max_exponent\": " << report.max_exponent << ",\n";
		out << "  \"classification\": \"" << report.classification_name << "\",\n";
		// ReSharper disable once CppDFAConstantConditions
		out << "  \"fallback_enabled\": " << (report.fallback_enabled ? "true" : "false") << ",\n";
		out << "  \"powl_path\": \"" << report.powl_path << "\",\n";
		// ReSharper disable once CppDFAConstantConditions
		out << "  \"mpfr_oracle_supported\": " << (report.mpfr_oracle_supported ? "true" : "false") << ",\n";
		out << "  \"mpfr_skip_reason\": \"" << ccm::test::oracle::json_escape(report.mpfr_skip_reason) << "\",\n";
		out << "  \"oracle_case_count\": " << report.oracle_case_count << ",\n";
		out << "  \"oracle_max_ulp\": " << report.oracle_max_ulp << ",\n";
		out << "  \"configuration_name\": \"" << ccm::test::oracle::configuration_name() << "\",\n";
		out << "  \"compiler\": \"" << ccm::test::oracle::json_escape(ccm::test::oracle::compiler_id()) << "\",\n";
		out << "  \"platform\": \"" << ccm::test::oracle::platform_id() << "\",\n";
		out << "  \"timestamp\": \"" << ccm::test::oracle::utc_timestamp() << "\"\n";
		out << "}\n";
	}
} // namespace

int main(int argc, char ** argv)
{
	const auto json_output = ccm::test::oracle::option_value(argc, argv, "--json-output=").value_or("powl-platform.json");

	platform_report report;
	report.sizeof_long_double  = sizeof(long double);
	report.digits			   = std::numeric_limits<long double>::digits;
	report.max_exponent		   = std::numeric_limits<long double>::max_exponent;
	report.classification	   = ccm::config::detect_long_double_format();
	report.classification_name = ccm::config::long_double_format_name(report.classification);
	report.fallback_enabled	   = ccm::config::reduced_precision_powl_fallback_enabled();
	report.powl_path		   = detect_powl_path();
	run_conservative_oracle(report);
	write_json(json_output, report);

	std::cout << "powl platform characterization\n";
	std::cout << "  sizeof(long double)=" << report.sizeof_long_double << " digits=" << report.digits << " max_exponent=" << report.max_exponent << '\n';
	std::cout << "  classification=" << report.classification_name << " fallback_enabled=" << (report.fallback_enabled ? "yes" : "no")
			  << " powl_path=" << report.powl_path << '\n';
	// ReSharper disable once CppDFAConstantConditions
	std::cout << "  mpfr_oracle_supported=" << (report.mpfr_oracle_supported ? "yes" : "no");
	if (!report.mpfr_skip_reason.empty()) { std::cout << " reason=" << report.mpfr_skip_reason; }
	std::cout << '\n';
	// ReSharper disable once CppDFAConstantConditions
	if (report.mpfr_oracle_supported)
	{
		std::cout << "  oracle_case_count=" << report.oracle_case_count << " oracle_max_ulp=" << report.oracle_max_ulp << '\n';
	}
	std::cout << "  json=" << json_output << '\n';

	// ReSharper disable once CppDFAConstantConditions
	return (report.mpfr_oracle_supported && report.oracle_max_ulp > 4) ? 1 : 0;
}
