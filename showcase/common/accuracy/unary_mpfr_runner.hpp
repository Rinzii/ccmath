#pragma once

#include "ccmath_paths.hpp"
#include "corpus.hpp"

#if defined(CCMATH_SHOWCASE_HAS_GCEM)
	#include <gcem.hpp>
#endif

#include "oracle/mpfr_oracle_harness.hpp"

#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace ccm::showcase::accuracy
{
	enum class unary_function
	{
		sqrt_fn,
		sin_fn,
	};

	inline std::string_view function_name(unary_function fn)
	{
		switch (fn)
		{
		case unary_function::sqrt_fn: return "sqrt";
		case unary_function::sin_fn: return "sin";
		}
		return "unknown";
	}

	inline std::vector<double> corpus_for(unary_function fn)
	{
		switch (fn)
		{
		case unary_function::sqrt_fn: return sqrt_corpus();
		case unary_function::sin_fn: return sin_corpus();
		}
		return {};
	}

	inline double std_reference(unary_function fn, double x)
	{
		switch (fn)
		{
		case unary_function::sqrt_fn: return std::sqrt(x);
		case unary_function::sin_fn: return std::sin(x);
		}
		return 0.0;
	}

	inline double mpfr_reference(unary_function fn, double x, mpfr_prec_t precision, mpfr_rnd_t rounding)
	{
		mpfr_t value;
		mpfr_t result;
		mpfr_init2(value, precision);
		mpfr_init2(result, precision);
		mpfr_set_d(value, x, rounding);
		switch (fn)
		{
		case unary_function::sqrt_fn: mpfr_sqrt(result, value, rounding); break;
		case unary_function::sin_fn: mpfr_sin(result, value, rounding); break;
		}
		const double out = mpfr_get_d(result, rounding);
		mpfr_clear(value);
		mpfr_clear(result);
		return out;
	}

	inline ccm::showcase::ccmath_path::path_support path_is_supported(unary_function fn, ccm::showcase::ccmath_path::path path)
	{
		switch (fn)
		{
		case unary_function::sqrt_fn: return ccm::showcase::ccmath_path::sqrt_is_supported(path);
		case unary_function::sin_fn: return ccm::showcase::ccmath_path::sin_is_supported(path);
		}
		return { false, "unknown function" };
	}

	inline double invoke_ccmath_path(unary_function fn, ccm::showcase::ccmath_path::path path, double x)
	{
		switch (fn)
		{
		case unary_function::sqrt_fn: return ccm::showcase::ccmath_path::invoke_sqrt(path, x);
		case unary_function::sin_fn: return ccm::showcase::ccmath_path::invoke_sin(path, x);
		}
		return 0.0;
	}

	struct path_report
	{
		std::string path;
		std::size_t case_count		   = 0;
		std::size_t failure_count	   = 0;
		std::uint64_t max_observed_ulp = 0;
	};

	inline int run_ccmath_path_campaign(
		unary_function fn, ccm::showcase::ccmath_path::path path, mpfr_prec_t precision, std::uint64_t max_ulp, const std::optional<std::string>& output_path)
	{
		const auto support = path_is_supported(fn, path);
		if (!support.supported)
		{
			std::cout << "skip path=" << ccm::showcase::ccmath_path::name(path) << " reason=" << support.skip_reason << '\n';
			return 0;
		}

		ccm::test::oracle::unary_run_summary<double> summary;
		std::vector<ccm::test::oracle::unary_failure_record<double>> events;
		const auto cases		   = corpus_for(fn);
		const std::string fn_label = std::string(function_name(fn));

		for (const double input : cases)
		{
			ccm::test::oracle::unary_case<double> test_case{ input, "showcase corpus" };
			(void)ccm::test::oracle::evaluate_unary_mpfr_case(
				test_case,
				fn_label,
				std::string(ccm::showcase::ccmath_path::name(path)),
				[fn, path](double value) { return invoke_ccmath_path(fn, path, value); },
				[fn](double value, mpfr_prec_t prec, mpfr_rnd_t rounding) { return mpfr_reference(fn, value, prec, rounding); },
				[fn](double value) { return std_reference(fn, value); },
				precision,
				max_ulp,
				0,
				summary,
				0,
				"showcase mismatch",
				&events);
		}

		std::cout << "accuracy function=" << fn_label << " backend=ccmath path=" << ccm::showcase::ccmath_path::name(path) << " cases=" << summary.case_count
				  << " failures=" << summary.failure_count << " max_ulp=" << summary.max_observed_ulp << '\n';

		if (output_path.has_value())
		{
			std::ofstream out(*output_path, std::ios::trunc);
			out << "[\n";
			for (std::size_t i = 0; i < events.size(); ++i)
			{
				const auto& event = events[i];
				out << "  {\"path\":\"" << event.path << "\",\"input_bits\":\"" << event.input_bits << "\",\"actual_bits\":\"" << event.actual_bits
					<< "\",\"expected_bits\":\"" << event.expected_bits << "\",\"ulp_distance\":" << event.ulp_distance << ",\"event_kind\":\""
					<< event.event_kind << "\"}" << (i + 1 < events.size() ? "," : "") << '\n';
			}
			out << "]\n";
		}

		return summary.failure_count > 0 ? 1 : 0;
	}

	inline int run_gcem_campaign(unary_function fn, mpfr_prec_t precision, std::uint64_t max_ulp, const std::optional<std::string>& output_path)
	{
#if !defined(CCMATH_SHOWCASE_HAS_GCEM)
		std::cout << "skip gcem accuracy: GCEM backend disabled\n";
		return 0;
#else
		ccm::test::oracle::unary_run_summary<double> summary;
		std::vector<ccm::test::oracle::unary_failure_record<double>> events;
		const auto cases		   = corpus_for(fn);
		const std::string fn_label = std::string(function_name(fn));

		for (const double input : cases)
		{
			ccm::test::oracle::unary_case<double> test_case{ input, "showcase corpus" };
			(void)ccm::test::oracle::evaluate_unary_mpfr_case(
				test_case,
				fn_label,
				"gcem",
				[fn](double value)
				{
					volatile double x = value;
					switch (fn)
					{
					case unary_function::sqrt_fn: return gcem::sqrt(x);
					case unary_function::sin_fn: return gcem::sin(x);
					}
					return 0.0;
				},
				[fn](double value, mpfr_prec_t prec, mpfr_rnd_t rounding) { return mpfr_reference(fn, value, prec, rounding); },
				[fn](double value) { return std_reference(fn, value); },
				precision,
				max_ulp,
				0,
				summary,
				0,
				"showcase mismatch",
				&events);
		}

		std::cout << "accuracy function=" << fn_label << " backend=gcem cases=" << summary.case_count << " failures=" << summary.failure_count
				  << " max_ulp=" << summary.max_observed_ulp << '\n';

		if (output_path.has_value())
		{
			std::ofstream out(*output_path, std::ios::trunc);
			out << "[\n";
			for (std::size_t i = 0; i < events.size(); ++i)
			{
				const auto& event = events[i];
				out << "  {\"path\":\"gcem\",\"input_bits\":\"" << event.input_bits << "\",\"actual_bits\":\"" << event.actual_bits << "\",\"expected_bits\":\""
					<< event.expected_bits << "\",\"ulp_distance\":" << event.ulp_distance << ",\"event_kind\":\"" << event.event_kind << "\"}"
					<< (i + 1 < events.size() ? "," : "") << '\n';
			}
			out << "]\n";
		}

		return summary.failure_count > 0 ? 1 : 0;
#endif
	}
} // namespace ccm::showcase::accuracy
