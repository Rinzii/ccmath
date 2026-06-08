#include "mpfr_pow_common.hpp"
#include "oracle/powl_path_reporting.hpp"

#include "ccmath/internal/config/type_support.hpp"

#include <fstream>
#include <iostream>

namespace
{
	using ccm::test::oracle::classify_powl_gen_path;
	using ccm::test::oracle::pow_case;
	using ccm::test::oracle::powl_path_is_native_phase1;
	using ccm::test::oracle::powl_path_is_reduced_precision;
	using ccm::test::oracle::powl_path_name;
	using ccm::test::oracle::PowlImplementationPath;

	std::vector<pow_case<long double>> build_ld64_cases()
	{
		const std::array<long double, 6> bases	   = { 0.25L, 0.5L, 1.0L, 2.0L, 3.0L, 10.0L };
		const std::array<long double, 6> exponents = { -2.0L, -0.5L, 0.0L, 0.5L, 2.0L, 3.0L };
		std::vector<pow_case<long double>> cases;
		for (long double base : bases)
		{
			for (long double exponent : exponents) { cases.push_back({ base, exponent, "ld64 conservative oracle grid" }); }
		}
		return cases;
	}

#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
	std::vector<pow_case<long double>> build_ld80_special_cases()
	{
		std::vector<pow_case<long double>> cases;
		const std::array<long double, 10> special_bases = {
			-std::numeric_limits<long double>::infinity(), -2.0L, -1.0L, -0.0L, 0.0L, 1.0L, 2.0L, 10.0L, std::numeric_limits<long double>::max(),
			std::numeric_limits<long double>::infinity(),
		};
		const std::array<long double, 9> special_exponents = {
			-std::numeric_limits<long double>::infinity(), -3.0L, -1.0L, -0.0L, 0.0L, 0.5L, 2.0L, 3.0L, std::numeric_limits<long double>::infinity(),
		};
		for (long double base : special_bases)
		{
			for (long double exponent : special_exponents) { cases.push_back({ base, exponent, "ld80 special-value matrix" }); }
		}
		return cases;
	}

	std::vector<pow_case<long double>> build_ld80_bounded_integer_cases()
	{
		std::vector<pow_case<long double>> cases;
		for (long double base : { 0.5L, 1.0001L, 2.0L, -2.0L, 10.0L })
		{
			for (long double exponent : { -1000.0L, -3.0L, -1.0L, 0.0L, 1.0L, 3.0L, 10.0L, 1000.0L, 0x1.0p40L })
			{
				cases.push_back({ base, exponent, "ld80 bounded integer exponent corpus" });
			}
		}
		for (long double exponent : { 0x1.0p62L, 0x1.0000000000001p62L, 0x1.0p63L }) { cases.push_back({ -1.0L, exponent, "ld80 parity threshold corpus" }); }
		return cases;
	}

	std::vector<pow_case<long double>> build_ld80_general_finite_cases()
	{
		std::vector<pow_case<long double>> cases;
		for (long double base : { 0.999L, 1.001L, 0.5L, 2.0L, 8.0L, 0x1.0p-20L, 0x1.0p20L })
		{
			for (long double exponent : { -1000.0L, -0.5L, 0.25L, 2.0L, 1000.0L })
			{
				if (base == 1.0L) { continue; }
				cases.push_back({ base, exponent, "ld80 general finite structured grid" });
			}
		}
		for (long double exponent : { 0x1.0p40L, 0x1.0p-40L, 16300.0L, -16300.0L })
		{
			cases.push_back({ 1.0000001L, exponent, "ld80 general finite exponent stress" });
		}
		return cases;
	}
#endif

	enum class powl_case_disposition
	{
		validated_native,
		validated_exceptional,
		skipped_unsupported,
		skipped_reduced_precision,
		failed,
	};

	struct powl_case_result
	{
		pow_case<long double> test_case;
		PowlImplementationPath path		  = PowlImplementationPath::UnknownUnsupported;
		powl_case_disposition disposition = powl_case_disposition::skipped_unsupported;
		std::uint64_t ulp_distance		  = 0;
		bool above_target				  = false;
		std::string notes;
	};

	struct powl_campaign_report
	{
		ccm::config::LongDoubleFormat format = ccm::config::LongDoubleFormat::Unknown;
		std::string corpus;
		bool fallback_enabled						= false;
		std::size_t case_count						= 0;
		std::size_t validated_native_count			= 0;
		std::size_t validated_exceptional_count		= 0;
		std::size_t skipped_unsupported_count		= 0;
		std::size_t skipped_reduced_precision_count = 0;
		std::size_t failure_count					= 0;
		std::size_t above_target_count				= 0;
		std::uint64_t max_observed_ulp				= 0;
		std::vector<powl_case_result> case_results;
	};

	powl_case_result evaluate_powl_case(const pow_case<long double> & test_case, mpfr_prec_t precision, std::uint64_t max_ulp, std::uint64_t target_ulp)
	{
		powl_case_result result;
		result.test_case = test_case;
		result.path		 = classify_powl_gen_path(test_case.base, test_case.exponent);

		if (result.path == PowlImplementationPath::Ld80Unsupported || result.path == PowlImplementationPath::Ld128Unsupported ||
			result.path == PowlImplementationPath::UnknownUnsupported)
		{
			result.disposition = powl_case_disposition::skipped_unsupported;
			return result;
		}

		if (powl_path_is_reduced_precision(result.path))
		{
			result.disposition = powl_case_disposition::skipped_reduced_precision;
			result.notes	   = "reduced-precision fallback path is not independent powl validation";
			return result;
		}

		const mpfr_rnd_t rounding  = ccm::test::oracle::current_mpfr_rounding_mode();
		const long double actual   = ccm::gen::pow_gen(test_case.base, test_case.exponent);
		const long double mpfr_ref = ccm::test::oracle::mpfr_pow_reference(test_case.base, test_case.exponent, precision, rounding);
		const long double std_ref  = std::pow(test_case.base, test_case.exponent);

		if (ccm::test::oracle::powl_exceptional_case(test_case.base, test_case.exponent) || result.path == PowlImplementationPath::Ld80SpecialCase)
		{
			if (!ccm::test::oracle::exceptional_or_zero_match(actual, std_ref))
			{
				result.disposition = powl_case_disposition::failed;
				result.notes	   = "exceptional mismatch vs std::pow";
				return result;
			}
			result.disposition = powl_case_disposition::validated_exceptional;
			return result;
		}

		std::uint64_t distance = 0;
		std::string notes;
		const bool pass		= ccm::test::oracle::oracle_match(actual, mpfr_ref, max_ulp, distance, notes);
		result.ulp_distance = distance;
		if (!pass)
		{
			result.disposition = powl_case_disposition::failed;
			result.notes	   = notes;
			return result;
		}

		result.above_target = distance > target_ulp;
		result.disposition	= powl_case_disposition::validated_native;
		return result;
	}

	void accumulate_report(powl_campaign_report & report, const powl_case_result & result)
	{
		report.case_results.push_back(result);
		switch (result.disposition)
		{
		case powl_case_disposition::validated_native:
			++report.validated_native_count;
			++report.case_count;
			if (result.ulp_distance >= report.max_observed_ulp) { report.max_observed_ulp = result.ulp_distance; }
			if (result.above_target) { ++report.above_target_count; }
			break;
		case powl_case_disposition::validated_exceptional:
			++report.validated_exceptional_count;
			++report.case_count;
			break;
		case powl_case_disposition::skipped_unsupported: ++report.skipped_unsupported_count; break;
		case powl_case_disposition::skipped_reduced_precision: ++report.skipped_reduced_precision_count; break;
		case powl_case_disposition::failed:
			++report.failure_count;
			++report.case_count;
			break;
		}
	}

	const char * disposition_name(powl_case_disposition value)
	{
		switch (value)
		{
		case powl_case_disposition::validated_native: return "validated_native";
		case powl_case_disposition::validated_exceptional: return "validated_exceptional";
		case powl_case_disposition::skipped_unsupported: return "skipped_unsupported";
		case powl_case_disposition::skipped_reduced_precision: return "skipped_reduced_precision";
		case powl_case_disposition::failed: return "failed";
		}
		return "failed";
	}

	void write_json(const std::string & path, const std::vector<powl_campaign_report> & reports, ccm::config::LongDoubleFormat format)
	{
		std::ofstream out(path, std::ios::trunc);
		out << "{\n";
		out << "  \"format\": \"" << ccm::config::long_double_format_name(format) << "\",\n";
		out << "  \"digits\": " << std::numeric_limits<long double>::digits << ",\n";
		out << "  \"max_exponent\": " << std::numeric_limits<long double>::max_exponent << ",\n";
		out << "  \"fallback_enabled\": " << (ccm::config::reduced_precision_powl_fallback_enabled() ? "true" : "false") << ",\n";
		out << "  \"configuration_name\": \"" << ccm::test::oracle::configuration_name() << "\",\n";
		out << "  \"compiler\": \"" << ccm::test::oracle::json_escape(ccm::test::oracle::compiler_id()) << "\",\n";
		out << "  \"platform\": \"" << ccm::test::oracle::platform_id() << "\",\n";
		out << "  \"timestamp\": \"" << ccm::test::oracle::utc_timestamp() << "\",\n";
		out << "  \"corpora\": [\n";
		for (std::size_t i = 0; i < reports.size(); ++i)
		{
			const auto & report = reports[i];
			out << "    {\n";
			out << "      \"corpus\": \"" << ccm::test::oracle::json_escape(report.corpus) << "\",\n";
			out << "      \"validated_native_count\": " << report.validated_native_count << ",\n";
			out << "      \"validated_exceptional_count\": " << report.validated_exceptional_count << ",\n";
			out << "      \"skipped_unsupported_count\": " << report.skipped_unsupported_count << ",\n";
			out << "      \"skipped_reduced_precision_count\": " << report.skipped_reduced_precision_count << ",\n";
			out << "      \"failure_count\": " << report.failure_count << ",\n";
			out << "      \"above_target_count\": " << report.above_target_count << ",\n";
			out << "      \"max_observed_ulp\": " << report.max_observed_ulp << ",\n";
			out << "      \"cases\": [\n";
			for (std::size_t j = 0; j < report.case_results.size(); ++j)
			{
				const auto & item = report.case_results[j];
				out << "        {\n";
				out << "          \"base_bits\": \"" << ccm::test::oracle::bits_hex(item.test_case.base) << "\",\n";
				out << "          \"exponent_bits\": \"" << ccm::test::oracle::bits_hex(item.test_case.exponent) << "\",\n";
				out << "          \"path\": \"" << powl_path_name(item.path) << "\",\n";
				out << "          \"reduced_precision\": " << (powl_path_is_reduced_precision(item.path) ? "true" : "false") << ",\n";
				out << "          \"native_phase1\": " << (powl_path_is_native_phase1(item.path) ? "true" : "false") << ",\n";
				out << "          \"disposition\": \"" << disposition_name(item.disposition) << "\",\n";
				out << "          \"ulp_distance\": " << item.ulp_distance << ",\n";
				out << "          \"notes\": \"" << ccm::test::oracle::json_escape(item.notes) << "\"\n";
				out << "        }" << (j + 1 < report.case_results.size() ? "," : "") << '\n';
			}
			out << "      ]\n";
			out << "    }" << (i + 1 < reports.size() ? "," : "") << '\n';
		}
		out << "  ]\n";
		out << "}\n";
	}

	powl_campaign_report run_corpus(const std::string & corpus_name,
									const std::vector<pow_case<long double>> & cases,
									const std::vector<int> & rounding_modes,
									ccm::config::LongDoubleFormat format,
									mpfr_prec_t precision,
									std::uint64_t max_ulp,
									std::uint64_t target_ulp)
	{
		powl_campaign_report report;
		report.format			= format;
		report.corpus			= corpus_name;
		report.fallback_enabled = ccm::config::reduced_precision_powl_fallback_enabled();

		for (const int rounding_mode : rounding_modes)
		{
			ccm::test::oracle::ScopedMpfrRoundingMode scope(rounding_mode);
			if (!scope) { continue; }
			for (const auto & test_case : cases) { accumulate_report(report, evaluate_powl_case(test_case, precision, max_ulp, target_ulp)); }
		}

		std::cout << "powl corpus=" << corpus_name << " format=" << ccm::config::long_double_format_name(format) << " rounding_modes=" << rounding_modes.size()
				  << " fallback_enabled=" << (report.fallback_enabled ? "yes" : "no") << " validated_native=" << report.validated_native_count
				  << " validated_exceptional=" << report.validated_exceptional_count << " skipped_unsupported=" << report.skipped_unsupported_count
				  << " skipped_reduced_precision=" << report.skipped_reduced_precision_count << " failures=" << report.failure_count
				  << " max_ulp=" << report.max_observed_ulp << " target_ulp=" << target_ulp << " above_target=" << report.above_target_count
				  << " oracle_precision=" << precision << " configuration=" << ccm::test::oracle::configuration_name()
				  << " compiler=" << ccm::test::oracle::compiler_id() << " platform=" << ccm::test::oracle::platform_id() << '\n';

		for (const auto & item : report.case_results)
		{
			if (item.disposition != powl_case_disposition::failed) { continue; }
			std::cout << "  FAIL base_bits=" << ccm::test::oracle::bits_hex(item.test_case.base)
					  << " exp_bits=" << ccm::test::oracle::bits_hex(item.test_case.exponent) << " path=" << powl_path_name(item.path)
					  << " disposition=" << disposition_name(item.disposition) << " notes=" << item.notes << '\n';
		}

		return report;
	}
} // namespace

int main(int argc, char ** argv)
{
	const auto json_output = ccm::test::oracle::option_value(argc, argv, "--json-output=").value_or("mpfr-powl-summary.json");

	if (!ccm::test::oracle::mpfr_long_double_supported())
	{
		std::cout << "SKIP mpfr powl harness: MPFR long double support unavailable\n";
		return 0;
	}

	// detect_long_double_format() is constexpr, so the format dispatch below is resolved at
	// compile time. Using if constexpr keeps MSVC /W4 from flagging the constant condition (C4127).
	constexpr auto format		= ccm::config::detect_long_double_format();
	const mpfr_prec_t precision = ccm::test::oracle::powl_oracle_precision(format);
	const std::uint64_t max_ulp = ccm::test::oracle::parse_option_or<std::uint64_t>(
		ccm::test::oracle::option_value(argc, argv, "--max-ulp="), [](const std::string & value) { return static_cast<std::uint64_t>(std::stoull(value)); }, 4);
	// Hard ceiling stays at 4 ULP. The correctly-rounded target is 0.5 ULP, which in the
	// integer ULP-distance metric versus the rounded MPFR reference is distance 0.
	const std::uint64_t target_ulp = ccm::test::oracle::parse_option_or<std::uint64_t>(
		ccm::test::oracle::option_value(argc, argv, "--target-ulp="),
		[](const std::string & value) { return static_cast<std::uint64_t>(std::stoull(value)); },
		0);
	const auto rounding_modes = ccm::test::oracle::parse_rounding_modes(argc, argv);
	mpfr_set_default_rounding_mode(MPFR_RNDN);

	std::cout << "powl MPFR harness format=" << ccm::config::long_double_format_name(format) << " digits=" << std::numeric_limits<long double>::digits
			  << " max_exponent=" << std::numeric_limits<long double>::max_exponent << " rounding_modes=" << rounding_modes.size()
			  << " fallback_enabled=" << (ccm::config::reduced_precision_powl_fallback_enabled() ? "yes" : "no") << '\n';

	std::vector<powl_campaign_report> reports;
	if constexpr (format == ccm::config::LongDoubleFormat::Double)
	{
		reports.push_back(run_corpus("ld64_conservative", build_ld64_cases(), rounding_modes, format, precision, max_ulp, target_ulp));
	}
	else if constexpr (format == ccm::config::LongDoubleFormat::X87Extended)
	{
		reports.push_back(run_corpus("ld80_special_cases", build_ld80_special_cases(), rounding_modes, format, precision, max_ulp, target_ulp));
		reports.push_back(run_corpus("ld80_bounded_integer", build_ld80_bounded_integer_cases(), rounding_modes, format, precision, max_ulp, target_ulp));
		reports.push_back(run_corpus("ld80_general_finite", build_ld80_general_finite_cases(), rounding_modes, format, precision, max_ulp, target_ulp));
	}
	else
	{
		std::cout << "powl MPFR harness: detection-only on format=" << ccm::config::long_double_format_name(format)
				  << " fallback_enabled=" << (ccm::config::reduced_precision_powl_fallback_enabled() ? "yes" : "no") << '\n';
	}

	if (!reports.empty())
	{
		write_json(json_output, reports, format);
		std::cout << "  json=" << json_output << '\n';
	}

	for (const auto & report : reports)
	{
		if (report.failure_count > 0) { return 1; }
	}

	return 0;
}
