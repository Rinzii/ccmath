#pragma once

#include "oracle_campaign_metadata.hpp"
#include "utils/pow_path_dispatch.hpp"

#include <algorithm>
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace ccm::test::oracle
{
	enum class campaign_mode
	{
		quick,
		extended,
		full,
		release,
	};

	template <typename T>
	struct pow_case
	{
		T base;
		T exponent;
		std::string provenance;
	};

	template <typename T>
	using binary_case = pow_case<T>;

	template <typename T>
	struct failure_record
	{
		std::string function_name;
		std::string input_type;
		std::string path;
		std::string provenance;
		T base;
		T exponent;
		T actual;
		T expected;
		std::string base_bits;
		std::string exponent_bits;
		std::string actual_bits;
		std::string expected_bits;
		std::uint64_t ulp_distance;
		std::string rounding_mode;
		unsigned long oracle_precision;
		std::string compiler;
		std::string platform;
		std::string fma_enabled;
		std::string builtin_available;
		std::string simd_available;
		std::string configuration_name;
		std::string optimization_mode;
		std::string search_mode;
		std::uint64_t seed = 0;
		std::string timestamp;
		std::string notes;
		// mpfr_hard_failure, mpfr_above_target, coremath_bit_mismatch, coremath_oracle_corrected
		std::string event_kind;
	};

	template <typename T>
	struct run_summary
	{
		std::size_t case_count				   = 0;
		std::size_t skipped_count			   = 0;
		std::size_t failure_count			   = 0;
		std::size_t mpfr_policy_mismatch_count = 0;
		// Cases where the primary oracle (cr_pow/cr_powf) disagrees with the function
		// but a higher-precision cross-check confirms the function is correctly rounded,
		// i.e. the oracle itself is wrong. These are not counted as failures.
		std::size_t oracle_corrected_count = 0;
		// Cases that pass the hard ULP ceiling but miss the correctly-rounded target
		// (more than 0.5 ULP from the real value). Tracked, not failed.
		std::size_t above_target_count = 0;
		std::uint64_t max_observed_ulp = 0;
		T worst_base{};
		T worst_exponent{};
		T worst_actual{};
		T worst_expected{};
	};

	template <typename T>
	struct campaign_report
	{
		std::string configuration_name;
		std::string path;
		std::string compiler;
		std::string platform;
		std::string optimization_mode;
		std::string fma_enabled;
		std::string builtin_available;
		std::string simd_available;
		std::string mode_name;
		std::vector<std::string> domains_covered;
		std::vector<std::string> domains_skipped;
		std::string oracle_policy;
		std::size_t case_count				   = 0;
		std::size_t skipped_count			   = 0;
		std::size_t failure_count			   = 0;
		std::size_t mpfr_policy_mismatch_count = 0;
		std::size_t oracle_corrected_count	   = 0;
		std::size_t above_target_count		   = 0;
		std::uint64_t max_observed_ulp		   = 0;
		std::string worst_base_bits;
		std::string worst_exponent_bits;
		std::string worst_actual_bits;
		std::string worst_expected_bits;
		std::uint64_t seed		 = 0;
		std::uint64_t elapsed_ms = 0;
		std::string timestamp;
	};

	inline std::string current_rounding_mode_name()
	{
		switch (std::fegetround())
		{
		case FE_TONEAREST: return "FE_TONEAREST";
		case FE_UPWARD: return "FE_UPWARD";
		case FE_DOWNWARD: return "FE_DOWNWARD";
		case FE_TOWARDZERO: return "FE_TOWARDZERO";
		default: return "unknown";
		}
	}

	inline std::string configuration_name()
	{
		return pow_configuration_name();
	}

	template <typename T>
	inline constexpr const char * builtin_status()
	{
		return pow_builtin_status<T>();
	}

	template <typename T>
	inline bool is_modeled_generic_pow_case(T base, T exponent)
	{
		return std::isfinite(base) && std::isfinite(exponent) && base > T{};
	}

	inline bool uses_public_mpfr_oracle(ccm::test::pow_path::validation_path path)
	{
#if defined(CCM_CONFIG_TEST_DISABLE_RUNTIME_BUILTIN_POW)
		(void)path;
		return false;
#else
		return path == ccm::test::pow_path::validation_path::public_default;
#endif
	}

	inline std::string oracle_policy_name(ccm::test::pow_path::validation_path path)
	{
		if (uses_public_mpfr_oracle(path)) { return "mpfr_full"; }
		return "kernel_finite_mpfr_with_std_exceptional";
	}

	inline std::string coremath_oracle_policy_name()
	{
		return "coremath_cr_finite_all_modes";
	}

	template <typename T>
	inline std::optional<std::string> kernel_path_skip_reason(ccm::test::pow_path::validation_path path, T base, T exponent)
	{
		if (uses_public_mpfr_oracle(path)) { return std::nullopt; }
		if (path == ccm::test::pow_path::validation_path::generic_modeled_domain)
		{
			if (!is_modeled_generic_pow_case(base, exponent)) { return "outside modeled generic positive finite domain"; }
			return std::nullopt;
		}
		if (!is_modeled_generic_pow_case(base, exponent)) { return "exceptional input outside generic kernel finite domain"; }
		return std::nullopt;
	}

	template <typename T>
	inline bool is_coremath_oracle_case(T base, T exponent)
	{
		if (!std::isfinite(base) || !std::isfinite(exponent)) { return false; }
		if (base == T{} || exponent == T{}) { return false; }
		if (base < T{} && static_cast<T>(std::trunc(static_cast<double>(exponent))) != exponent) { return false; }
		return true;
	}

	template <typename T>
	inline bool oracle_fp_bits_match(T actual, T expected)
	{
		using fp_bits = ccm::support::fp::FPBits<T>;
		return fp_bits(actual).uintval() == fp_bits(expected).uintval();
	}

	template <typename T>
	inline void record_worst_case(run_summary<T> & summary, std::uint64_t ulp_distance, T base, T exponent, T actual, T expected)
	{
		if (ulp_distance >= summary.max_observed_ulp)
		{
			summary.max_observed_ulp = ulp_distance;
			summary.worst_base		 = base;
			summary.worst_exponent	 = exponent;
			summary.worst_actual	 = actual;
			summary.worst_expected	 = expected;
		}
	}

	template <typename T>
	inline failure_record<T> make_failure_record(std::string_view function_name,
												 std::string_view path_name,
												 std::string_view provenance,
												 T base,
												 T exponent,
												 T actual,
												 T expected,
												 std::uint64_t ulp_distance,
												 std::string_view rounding_mode,
												 unsigned long oracle_precision,
												 std::uint64_t seed,
												 std::string_view search_mode,
												 std::string_view notes,
												 std::string_view event_kind = {})
	{
		return failure_record<T>{
			std::string(function_name),
			scalar_type_name<T>(),
			std::string(path_name),
			std::string(provenance),
			base,
			exponent,
			actual,
			expected,
			bits_hex(base),
			bits_hex(exponent),
			bits_hex(actual),
			bits_hex(expected),
			ulp_distance,
			std::string(rounding_mode),
			oracle_precision,
			compiler_id(),
			platform_id(),
			fma_status(),
			builtin_status<T>(),
			simd_status(),
			configuration_name(),
			optimization_mode(),
			std::string(search_mode),
			seed,
			utc_timestamp(),
			std::string(notes),
			std::string(event_kind),
		};
	}

	template <typename T>
	inline bool is_hard_oracle_failure(const failure_record<T> & event)
	{
		return event.event_kind == "mpfr_hard_failure" || event.event_kind == "coremath_bit_mismatch";
	}

	template <typename T>
	inline bool has_hard_oracle_failure(const std::vector<failure_record<T>> & events)
	{
		for (const auto & event : events)
		{
			if (is_hard_oracle_failure(event)) { return true; }
		}
		return false;
	}

	template <typename T>
	inline void write_failure_json(const std::string & output_path, const std::vector<failure_record<T>> & failures)
	{
		std::ofstream out(output_path, std::ios::trunc);
		out << "[\n";
		for (std::size_t i = 0; i < failures.size(); ++i)
		{
			const auto & failure = failures[i];
			out << "  {\n";
			out << "    \"function_name\": \"" << json_escape(failure.function_name) << "\",\n";
			out << "    \"input_type\": \"" << json_escape(failure.input_type) << "\",\n";
			out << "    \"path\": \"" << json_escape(failure.path) << "\",\n";
			out << "    \"provenance\": \"" << json_escape(failure.provenance) << "\",\n";
			out << "    \"base_bits\": \"" << failure.base_bits << "\",\n";
			out << "    \"exponent_bits\": \"" << failure.exponent_bits << "\",\n";
			out << "    \"actual_bits\": \"" << failure.actual_bits << "\",\n";
			out << "    \"expected_bits\": \"" << failure.expected_bits << "\",\n";
			out << "    \"ulp_distance\": " << failure.ulp_distance << ",\n";
			out << "    \"rounding_mode\": \"" << json_escape(failure.rounding_mode) << "\",\n";
			out << "    \"oracle_precision\": " << failure.oracle_precision << ",\n";
			out << "    \"compiler\": \"" << json_escape(failure.compiler) << "\",\n";
			out << "    \"platform\": \"" << json_escape(failure.platform) << "\",\n";
			out << "    \"fma_enabled\": \"" << failure.fma_enabled << "\",\n";
			out << "    \"builtin_available\": \"" << failure.builtin_available << "\",\n";
			out << "    \"simd_available\": \"" << failure.simd_available << "\",\n";
			out << "    \"configuration_name\": \"" << json_escape(failure.configuration_name) << "\",\n";
			out << "    \"optimization_mode\": \"" << json_escape(failure.optimization_mode) << "\",\n";
			out << "    \"search_mode\": \"" << json_escape(failure.search_mode) << "\",\n";
			out << "    \"seed\": " << failure.seed << ",\n";
			out << "    \"timestamp\": \"" << json_escape(failure.timestamp) << "\",\n";
			out << "    \"event_kind\": \"" << json_escape(failure.event_kind) << "\",\n";
			out << "    \"notes\": \"" << json_escape(failure.notes) << "\"\n";
			out << "  }" << (i + 1 == failures.size() ? "\n" : ",\n");
		}
		out << "]\n";
	}

	// Echo hard-failure records to stdout so CI logs surface the offending input without an
	// uploaded artifact. The JSON event log keeps the full record; this is the at-a-glance view.
	template <typename T>
	inline void print_hard_failures(const std::vector<failure_record<T>> & events)
	{
		for (const auto & event : events)
		{
			if (!is_hard_oracle_failure(event)) { continue; }
			std::cout << "HARD FAILURE " << event.event_kind << " fn=" << event.function_name << " path=" << event.path << " rounding=" << event.rounding_mode
					  << " fma=" << event.fma_enabled << " base=" << event.base_bits << " exponent=" << event.exponent_bits << " actual=" << event.actual_bits
					  << " expected=" << event.expected_bits << " ulp=" << event.ulp_distance << " provenance=\"" << event.provenance << "\" notes=\""
					  << event.notes << "\"\n";
		}
	}

	inline std::optional<std::string> option_value(int argc, char ** argv, std::string_view prefix)
	{
		for (int i = 1; i < argc; ++i)
		{
			const std::string_view arg(argv[i]);
			if (arg.rfind(prefix, 0) == 0) { return std::string(arg.substr(prefix.size())); }
		}
		return std::nullopt;
	}

	inline std::optional<std::string> resolve_event_log_path(int argc, char ** argv)
	{
		if (const auto log_output = option_value(argc, argv, "--log-output=")) { return *log_output; }
		if (const auto json_output = option_value(argc, argv, "--json-output=")) { return *json_output; }
		return std::nullopt;
	}

	inline bool has_flag(int argc, char ** argv, std::string_view flag)
	{
		for (int i = 1; i < argc; ++i)
		{
			if (std::string_view(argv[i]) == flag) { return true; }
		}
		return false;
	}

	template <typename T, typename Parser>
	inline T parse_option_or(const std::optional<std::string> & raw_value, Parser parser, T fallback)
	{
		return raw_value.has_value() ? parser(*raw_value) : fallback;
	}

	inline campaign_mode parse_mode(const std::optional<std::string> & raw_mode)
	{
		if (!raw_mode.has_value() || *raw_mode == "quick") { return campaign_mode::quick; }
		if (*raw_mode == "extended") { return campaign_mode::extended; }
		if (*raw_mode == "release" || *raw_mode == "full") { return campaign_mode::release; }
		return campaign_mode::quick;
	}

	inline std::string mode_name(campaign_mode mode)
	{
		switch (mode)
		{
		case campaign_mode::quick: return "quick";
		case campaign_mode::extended: return "extended";
		case campaign_mode::full:
		case campaign_mode::release: return "release";
		}
		return "quick";
	}

	inline std::vector<ccm::test::pow_path::validation_path> parse_paths(int argc, char ** argv)
	{
		std::vector<ccm::test::pow_path::validation_path> paths;
		const auto raw = option_value(argc, argv, "--path=");
		if (raw.has_value())
		{
			std::string_view view = *raw;
			while (!view.empty())
			{
				const std::size_t comma		 = view.find(',');
				const std::string_view token = view.substr(0, comma);
				if (auto parsed = ccm::test::pow_path::parse_path(token)) { paths.push_back(*parsed); }
				if (comma == std::string_view::npos) { break; }
				view.remove_prefix(comma + 1);
			}
		}
		if (paths.empty()) { paths.push_back(ccm::test::pow_path::validation_path::public_default); }
		return paths;
	}

	inline std::vector<int> parse_rounding_modes(int argc, char ** argv)
	{
		const auto raw = option_value(argc, argv, "--rounding-modes=");
		if (!raw.has_value() || *raw == "all") { return { FE_TONEAREST, FE_UPWARD, FE_DOWNWARD, FE_TOWARDZERO }; }

		std::vector<int> modes;
		std::string_view view = *raw;
		while (!view.empty())
		{
			const std::size_t comma		 = view.find(',');
			const std::string_view token = view.substr(0, comma);
			if (token == "nearest" || token == "FE_TONEAREST") { modes.push_back(FE_TONEAREST); }
			else if (token == "upward" || token == "FE_UPWARD") { modes.push_back(FE_UPWARD); }
			else if (token == "downward" || token == "FE_DOWNWARD") { modes.push_back(FE_DOWNWARD); }
			else if (token == "towardzero" || token == "FE_TOWARDZERO") { modes.push_back(FE_TOWARDZERO); }
			if (comma == std::string_view::npos) { break; }
			view.remove_prefix(comma + 1);
		}
		if (modes.empty()) { modes = { FE_TONEAREST, FE_UPWARD, FE_DOWNWARD, FE_TOWARDZERO }; }
		return modes;
	}

	template <typename T>
	inline void write_campaign_summary_json(const std::string & output_path, const campaign_report<T> & report)
	{
		std::ofstream out(output_path, std::ios::trunc);
		out << "{\n";
		out << "  \"configuration_name\": \"" << json_escape(report.configuration_name) << "\",\n";
		out << "  \"path\": \"" << json_escape(report.path) << "\",\n";
		out << "  \"compiler\": \"" << json_escape(report.compiler) << "\",\n";
		out << "  \"platform\": \"" << json_escape(report.platform) << "\",\n";
		out << "  \"optimization_mode\": \"" << json_escape(report.optimization_mode) << "\",\n";
		out << "  \"fma_enabled\": \"" << report.fma_enabled << "\",\n";
		out << "  \"builtin_available\": \"" << report.builtin_available << "\",\n";
		out << "  \"simd_available\": \"" << report.simd_available << "\",\n";
		out << "  \"mode_name\": \"" << json_escape(report.mode_name) << "\",\n";
		out << "  \"domains_covered\": [";
		for (std::size_t i = 0; i < report.domains_covered.size(); ++i)
		{
			out << "\"" << json_escape(report.domains_covered[i]) << "\"";
			if (i + 1 < report.domains_covered.size()) { out << ", "; }
		}
		out << "],\n";
		out << "  \"domains_skipped\": [";
		for (std::size_t i = 0; i < report.domains_skipped.size(); ++i)
		{
			out << "\"" << json_escape(report.domains_skipped[i]) << "\"";
			if (i + 1 < report.domains_skipped.size()) { out << ", "; }
		}
		out << "],\n";
		out << "  \"case_count\": " << report.case_count << ",\n";
		out << "  \"skipped_count\": " << report.skipped_count << ",\n";
		out << "  \"failure_count\": " << report.failure_count << ",\n";
		out << "  \"mpfr_policy_mismatch_count\": " << report.mpfr_policy_mismatch_count << ",\n";
		out << "  \"oracle_corrected_count\": " << report.oracle_corrected_count << ",\n";
		out << "  \"above_target_count\": " << report.above_target_count << ",\n";
		out << "  \"oracle_policy\": \"" << json_escape(report.oracle_policy) << "\",\n";
		out << "  \"max_observed_ulp\": " << report.max_observed_ulp << ",\n";
		out << "  \"worst_base_bits\": \"" << report.worst_base_bits << "\",\n";
		out << "  \"worst_exponent_bits\": \"" << report.worst_exponent_bits << "\",\n";
		out << "  \"worst_actual_bits\": \"" << report.worst_actual_bits << "\",\n";
		out << "  \"worst_expected_bits\": \"" << report.worst_expected_bits << "\",\n";
		out << "  \"seed\": " << report.seed << ",\n";
		out << "  \"elapsed_ms\": " << report.elapsed_ms << ",\n";
		out << "  \"timestamp\": \"" << json_escape(report.timestamp) << "\"\n";
		out << "}\n";
	}

	template <typename T>
	inline campaign_report<T> make_campaign_report(std::string_view path_name,
												   ccm::test::pow_path::validation_path path,
												   campaign_mode mode,
												   const run_summary<T> & summary,
												   std::uint64_t seed,
												   std::uint64_t elapsed_ms,
												   const std::vector<std::string> & domains_covered,
												   const std::vector<std::string> & domains_skipped)
	{
		return campaign_report<T>{
			configuration_name(),
			std::string(path_name),
			compiler_id(),
			platform_id(),
			optimization_mode(),
			fma_status(),
			builtin_status<T>(),
			simd_status(),
			mode_name(mode),
			domains_covered,
			domains_skipped,
			oracle_policy_name(path),
			summary.case_count,
			summary.skipped_count,
			summary.failure_count,
			summary.mpfr_policy_mismatch_count,
			summary.oracle_corrected_count,
			summary.above_target_count,
			summary.max_observed_ulp,
			bits_hex(summary.worst_base),
			bits_hex(summary.worst_exponent),
			bits_hex(summary.worst_actual),
			bits_hex(summary.worst_expected),
			seed,
			elapsed_ms,
			utc_timestamp(),
		};
	}

	template <typename T>
	inline campaign_report<T> make_coremath_campaign_report(std::string_view path_name,
															campaign_mode mode,
															const run_summary<T> & summary,
															std::uint64_t seed,
															std::uint64_t elapsed_ms,
															const std::vector<std::string> & domains_covered,
															const std::vector<std::string> & domains_skipped)
	{
		return campaign_report<T>{
			configuration_name(),
			std::string(path_name),
			compiler_id(),
			platform_id(),
			optimization_mode(),
			fma_status(),
			builtin_status<T>(),
			simd_status(),
			mode_name(mode),
			domains_covered,
			domains_skipped,
			coremath_oracle_policy_name(),
			summary.case_count,
			summary.skipped_count,
			summary.failure_count,
			summary.mpfr_policy_mismatch_count,
			summary.oracle_corrected_count,
			summary.above_target_count,
			summary.max_observed_ulp,
			bits_hex(summary.worst_base),
			bits_hex(summary.worst_exponent),
			bits_hex(summary.worst_actual),
			bits_hex(summary.worst_expected),
			seed,
			elapsed_ms,
			utc_timestamp(),
		};
	}

	template <typename T, typename ExecuteFn, typename ReportFn, typename PrintFn>
	inline void run_path_campaign(ccm::test::pow_path::validation_path path,
								  run_summary<T> & summary,
								  std::string_view summary_prefix,
								  ExecuteFn execute_cases,
								  ReportFn build_report,
								  PrintFn print_report)
	{
		const auto support = ccm::test::pow_path::path_is_supported<T>(path);
		if (!support.supported)
		{
			std::cout << "SKIP path=" << ccm::test::pow_path::path_name(path) << " reason=" << support.skip_reason << '\n';
			return;
		}

		const auto started = std::chrono::steady_clock::now();
		execute_cases(summary);
		const auto elapsed			   = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - started);
		const auto report			   = build_report(summary, static_cast<std::uint64_t>(elapsed.count()));
		const std::string summary_path = std::string(summary_prefix) + report.path + "-summary.json";
		write_campaign_summary_json(summary_path, report);
		print_report(report, summary_path);
	}

	template <typename T>
	inline void add_random_cases(std::vector<pow_case<T>> & cases, std::uint64_t seed, std::size_t count, const char * provenance)
	{
		std::mt19937_64 rng(seed);
		for (std::size_t i = 0; i < count; ++i)
		{
			if constexpr (std::is_same_v<T, float>)
			{
				const auto base_bits = static_cast<std::uint32_t>(rng());
				const auto exp_bits	 = static_cast<std::uint32_t>(rng());
				cases.push_back(pow_case<T>{
					ccm::support::bit_cast<float>(base_bits),
					ccm::support::bit_cast<float>(exp_bits),
					provenance,
				});
			}
			else
			{
				cases.push_back(pow_case<T>{
					ccm::support::bit_cast<double>(rng()),
					ccm::support::bit_cast<double>(rng()),
					provenance,
				});
			}
		}
	}
} // namespace ccm::test::oracle
