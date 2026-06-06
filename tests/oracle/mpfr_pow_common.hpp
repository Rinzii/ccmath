#pragma once

#include "ccmath/ccmath.hpp"
#include "ccmath/internal/config/long_double_format.hpp"
#include "ccmath/internal/math/generic/builtins/power/pow.hpp"
#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "utils/pow_path_dispatch.hpp"
#include "utils/ulp_distance.hpp"
#include "utils/worst_case_samples.hpp"

#include <mpfr.h>

#include <algorithm>
#include <array>
#include <cfenv>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <limits>
#include <optional>
#include <random>
#include <set>
#include <sstream>
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
	};

	template <typename T>
	struct run_summary
	{
		std::size_t case_count				   = 0;
		std::size_t skipped_count			   = 0;
		std::size_t failure_count			   = 0;
		std::size_t mpfr_policy_mismatch_count = 0;
		std::uint64_t max_observed_ulp		   = 0;
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
		std::uint64_t max_observed_ulp		   = 0;
		std::string worst_base_bits;
		std::string worst_exponent_bits;
		std::string worst_actual_bits;
		std::string worst_expected_bits;
		std::uint64_t seed		 = 0;
		std::uint64_t elapsed_ms = 0;
		std::string timestamp;
	};

	inline std::string compiler_id()
	{
#if defined(__clang__)
		return std::string("clang ") + __clang_version__;
#elif defined(__GNUC__)
		return std::string("gcc ") + __VERSION__;
#elif defined(_MSC_FULL_VER)
		return "msvc " + std::to_string(_MSC_FULL_VER);
#elif defined(_MSC_VER)
		return "msvc " + std::to_string(_MSC_VER);
#else
		return "unknown";
#endif
	}

	inline std::string platform_id()
	{
#if defined(__APPLE__)
		return "apple";
#elif defined(_WIN32)
		return "windows";
#elif defined(__linux__)
		return "linux";
#else
		return "unknown";
#endif
	}

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

	inline mpfr_rnd_t current_mpfr_rounding_mode()
	{
		switch (std::fegetround())
		{
		case FE_UPWARD: return MPFR_RNDU;
		case FE_DOWNWARD: return MPFR_RNDD;
		case FE_TOWARDZERO: return MPFR_RNDZ;
		case FE_TONEAREST:
		default: return MPFR_RNDN;
		}
	}

	inline constexpr const char * fma_status()
	{
#if defined(CCMATH_TARGET_CPU_HAS_FMA)
		return "on";
#else
		return "off";
#endif
	}

	inline std::string configuration_name()
	{
#ifdef CCM_CONFIG_TEST_POW_VALIDATION_NAME
		return CCM_CONFIG_TEST_POW_VALIDATION_NAME;
#else
		return "default";
#endif
	}

	inline std::string optimization_mode()
	{
#if defined(NDEBUG)
		return "release";
#else
		return "debug";
#endif
	}

	inline std::string utc_timestamp()
	{
		const auto now	= std::chrono::system_clock::now();
		const auto time = std::chrono::system_clock::to_time_t(now);
		std::tm tm{};
#if defined(_WIN32)
		gmtime_s(&tm, &time);
#else
		gmtime_r(&time, &tm);
#endif
		std::ostringstream oss;
		oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
		return oss.str();
	}

	template <typename T>
	inline constexpr const char * builtin_status()
	{
		return ccm::builtin::has_runtime_pow<T> ? "available" : "unavailable";
	}

	inline constexpr const char * simd_status()
	{
#if defined(CCMATH_HAS_SIMD)
		return "compiled";
#else
		return "unavailable";
#endif
	}

	template <typename T>
	inline constexpr const char * scalar_type_name()
	{
		if constexpr (std::is_same_v<T, float>) { return "float"; }
		else if constexpr (std::is_same_v<T, double>) { return "double"; }
		else if constexpr (std::is_same_v<T, long double>) { return "long double"; }
		else { return "unknown"; }
	}

	inline bool mpfr_long_double_supported()
	{
#if defined(MPFR_VERSION) && (MPFR_VERSION >= MPFR_VERSION_NUM(3, 0, 0))
		return true;
#else
		return false;
#endif
	}

	inline mpfr_prec_t powl_oracle_precision(ccm::config::LongDoubleFormat format)
	{
		switch (format)
		{
		case ccm::config::LongDoubleFormat::Double: return 256;
		// 128 bits double-rounds when reducing to the 64-bit significand for values that sit within
		// ~2^-128 of a rounding boundary (e.g. LDBL_MAX^0.5, which is just below 2^8192), producing
		// spurious failures even for correctly rounded results. 256 bits matches the other formats.
		case ccm::config::LongDoubleFormat::X87Extended: return 256;
		case ccm::config::LongDoubleFormat::IEEEBinary128: return 256;
		case ccm::config::LongDoubleFormat::Unknown: return 256;
		}
		return 256;
	}

	inline bool powl_exceptional_case(long double base, long double exponent)
	{
		if (std::isnan(base) || std::isnan(exponent)) { return true; }
		if (!std::isfinite(base) || !std::isfinite(exponent)) { return true; }
		if (base == 0.0L) { return true; }
		if (base < 0.0L) { return true; }
		if (exponent == 0.0L) { return true; }
		if (base == 1.0L) { return true; }
		return false;
	}

	template <typename T>
	inline std::string bits_hex(T value)
	{
		using fp_bits	   = ccm::support::fp::FPBits<T>;
		using storage_type = typename fp_bits::storage_type;

		const storage_type bits = fp_bits(value).uintval();
		std::ostringstream oss;
		oss << "0x" << std::hex << std::setfill('0');
		if constexpr (sizeof(storage_type) <= sizeof(std::uint64_t))
		{
			oss << std::setw(static_cast<int>(sizeof(storage_type) * 2)) << static_cast<std::uint64_t>(bits);
		}
		else
		{
			const auto hi = static_cast<std::uint64_t>(bits >> 64);
			const auto lo = static_cast<std::uint64_t>(bits);
			oss << std::setw(16) << hi << std::setw(16) << lo;
		}
		return oss.str();
	}

	inline std::string json_escape(std::string_view input)
	{
		std::ostringstream oss;
		for (const char ch : input)
		{
			switch (ch)
			{
			case '\\': oss << "\\\\"; break;
			case '"': oss << "\\\""; break;
			case '\n': oss << "\\n"; break;
			default: oss << ch; break;
			}
		}
		return oss.str();
	}

	template <typename T>
	inline void mpfr_set_scalar(mpfr_t value, T input)
	{
		if constexpr (std::is_same_v<T, float>) { mpfr_set_flt(value, input, MPFR_RNDN); }
		else if constexpr (std::is_same_v<T, double>) { mpfr_set_d(value, input, MPFR_RNDN); }
		else if constexpr (std::is_same_v<T, long double>) { mpfr_set_ld(value, input, MPFR_RNDN); }
	}

	template <typename T>
	inline T mpfr_get_scalar(const mpfr_t value, mpfr_rnd_t rounding)
	{
		if constexpr (std::is_same_v<T, float>) { return mpfr_get_flt(value, rounding); }
		else if constexpr (std::is_same_v<T, double>) { return mpfr_get_d(value, rounding); }
		else if constexpr (std::is_same_v<T, long double>) { return mpfr_get_ld(value, rounding); }
	}

	template <typename T>
	inline T mpfr_pow_reference(T base, T exponent, mpfr_prec_t precision, mpfr_rnd_t rounding)
	{
		mpfr_t base_mpfr;
		mpfr_t exp_mpfr;
		mpfr_t result_mpfr;
		mpfr_init2(base_mpfr, precision);
		mpfr_init2(exp_mpfr, precision);
		mpfr_init2(result_mpfr, precision);

		mpfr_set_scalar(base_mpfr, base);
		mpfr_set_scalar(exp_mpfr, exponent);
		mpfr_pow(result_mpfr, base_mpfr, exp_mpfr, rounding);

		const T result = mpfr_get_scalar<T>(result_mpfr, rounding);
		mpfr_clear(base_mpfr);
		mpfr_clear(exp_mpfr);
		mpfr_clear(result_mpfr);
		return result;
	}

	template <typename T>
	inline bool is_modeled_generic_pow_case(T base, T exponent)
	{
		return std::isfinite(base) && std::isfinite(exponent) && base > T{};
	}

	template <typename T>
	inline bool oracle_match(T actual, T expected, std::uint64_t max_ulp, std::uint64_t & ulp_distance, std::string & notes)
	{
		const auto distance = ccm::test::ulp::classify_distance(actual, expected);
		ulp_distance		= distance.distance;

		if (std::isnan(expected))
		{
			if (!std::isnan(actual))
			{
				notes = "expected NaN from MPFR";
				return false;
			}
			return true;
		}
		if (std::isinf(expected))
		{
			if (actual != expected)
			{
				notes = "infinity mismatch";
				return false;
			}
			return true;
		}
		if (actual == T{} && expected == T{})
		{
			if (std::signbit(actual) != std::signbit(expected))
			{
				notes = "signed-zero mismatch";
				return false;
			}
			return true;
		}
		if (distance.kind != ccm::test::ulp::relation::finite)
		{
			notes = "non-finite mismatch";
			return false;
		}
		if (distance.distance > max_ulp)
		{
			notes = "ULP threshold exceeded";
			return false;
		}
		return true;
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
	inline bool exceptional_or_zero_match(T actual, T reference)
	{
		if (std::isnan(actual)) { return std::isnan(reference); }
		if (std::isinf(actual)) { return actual == reference; }
		if (actual == T{} && reference == T{}) { return std::signbit(actual) == std::signbit(reference); }
		return actual == reference;
	}

	template <typename T>
	inline bool is_exceptional_or_zero_result(T value)
	{
		return std::isnan(value) || std::isinf(value) || value == T{};
	}

	template <typename T, typename Fn>
	inline std::optional<failure_record<T>> evaluate_case(const pow_case<T> & test_case,
														  std::string_view function_name,
														  std::string_view path_name,
														  ccm::test::pow_path::validation_path path,
														  Fn fn,
														  mpfr_prec_t oracle_precision,
														  std::uint64_t max_ulp,
														  run_summary<T> & summary,
														  std::uint64_t seed		   = 0,
														  std::string_view search_mode = {})
	{
		if (const auto skip = kernel_path_skip_reason(path, test_case.base, test_case.exponent))
		{
			++summary.skipped_count;
			return std::nullopt;
		}

		++summary.case_count;

		const mpfr_rnd_t rounding = current_mpfr_rounding_mode();
		const T actual			  = fn(test_case.base, test_case.exponent);

		if (!uses_public_mpfr_oracle(path))
		{
			const T std_ref	 = std::pow(test_case.base, test_case.exponent);
			const T mpfr_ref = mpfr_pow_reference(test_case.base, test_case.exponent, oracle_precision, rounding);

			if (is_exceptional_or_zero_result(actual) || is_exceptional_or_zero_result(std_ref))
			{
				if (exceptional_or_zero_match(actual, std_ref))
				{
					if (!exceptional_or_zero_match(actual, mpfr_ref)) { ++summary.mpfr_policy_mismatch_count; }
					return std::nullopt;
				}

				++summary.failure_count;
				return failure_record<T>{
					std::string(function_name),
					scalar_type_name<T>(),
					std::string(path_name),
					test_case.provenance,
					test_case.base,
					test_case.exponent,
					actual,
					std_ref,
					bits_hex(test_case.base),
					bits_hex(test_case.exponent),
					bits_hex(actual),
					bits_hex(std_ref),
					0,
					current_rounding_mode_name(),
					static_cast<unsigned long>(oracle_precision),
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
					"exceptional mismatch vs std::pow",
				};
			}

			std::uint64_t distance = 0;
			std::string notes;
			const bool pass = oracle_match(actual, mpfr_ref, max_ulp, distance, notes);
			if (distance >= summary.max_observed_ulp)
			{
				summary.max_observed_ulp = distance;
				summary.worst_base		 = test_case.base;
				summary.worst_exponent	 = test_case.exponent;
				summary.worst_actual	 = actual;
				summary.worst_expected	 = mpfr_ref;
			}
			if (pass) { return std::nullopt; }

			++summary.failure_count;
			return failure_record<T>{
				std::string(function_name),
				scalar_type_name<T>(),
				std::string(path_name),
				test_case.provenance,
				test_case.base,
				test_case.exponent,
				actual,
				mpfr_ref,
				bits_hex(test_case.base),
				bits_hex(test_case.exponent),
				bits_hex(actual),
				bits_hex(mpfr_ref),
				distance,
				current_rounding_mode_name(),
				static_cast<unsigned long>(oracle_precision),
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
				notes,
			};
		}

		const T expected = mpfr_pow_reference(test_case.base, test_case.exponent, oracle_precision, rounding);

		std::uint64_t distance = 0;
		std::string notes;
		const bool pass = oracle_match(actual, expected, max_ulp, distance, notes);
		if (distance >= summary.max_observed_ulp)
		{
			summary.max_observed_ulp = distance;
			summary.worst_base		 = test_case.base;
			summary.worst_exponent	 = test_case.exponent;
			summary.worst_actual	 = actual;
			summary.worst_expected	 = expected;
		}
		if (pass) { return std::nullopt; }

		++summary.failure_count;
		return failure_record<T>{
			std::string(function_name),
			scalar_type_name<T>(),
			std::string(path_name),
			test_case.provenance,
			test_case.base,
			test_case.exponent,
			actual,
			expected,
			bits_hex(test_case.base),
			bits_hex(test_case.exponent),
			bits_hex(actual),
			bits_hex(expected),
			distance,
			current_rounding_mode_name(),
			static_cast<unsigned long>(oracle_precision),
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
			notes,
		};
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
			out << "    \"notes\": \"" << json_escape(failure.notes) << "\"\n";
			out << "  }" << (i + 1 == failures.size() ? "\n" : ",\n");
		}
		out << "]\n";
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
