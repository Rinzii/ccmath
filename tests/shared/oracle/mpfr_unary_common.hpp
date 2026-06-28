/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#pragma once

#include "mpfr_oracle_harness.hpp"
#include "oracle_campaign_common.hpp"

#include <mpfr.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <vector>

// Elementary-function campaign against a correctly-rounded MPFR reference, generic over arity.
//
// The inputs of a case are a std::array<T, N>, so unary, binary, and ternary functions all run
// through one evaluation, summary, report, and JSON path. Only three things vary with arity: the
// ccm kernel call, the MPFR reference call (each MPFR function has a fixed signature, so a small
// make_*_ref helper adapts it), and how the corpus is built. Round-to-nearest binary32 with N == 1
// can be swept exhaustively over every bit pattern; everything else is sampled.
//
// The namespace keeps its historical name. It is no longer unary-only.
namespace ccm::test::oracle::unary_oracle
{
	// Per-thread MPFR scratch with N operands plus an output, reused across evaluations so the
	// exhaustive sweep does not allocate on every one of billions of references. Precision is fixed
	// for a campaign, so the grow case only runs on the first call.
	template <std::size_t N> struct mpfr_scratch
	{
		mpfr_t in[N];
		mpfr_t out;
		mpfr_prec_t precision = 0;

		mpfr_scratch()								   = default;
		mpfr_scratch(const mpfr_scratch &)			   = delete;
		mpfr_scratch & operator=(const mpfr_scratch &) = delete;
		~mpfr_scratch()
		{
			if (precision != 0)
			{
				for (std::size_t i = 0; i < N; ++i)
				{
					mpfr_clear(in[i]);
				}
				mpfr_clear(out);
			}
		}

		void ensure(mpfr_prec_t wanted)
		{
			if (precision >= wanted)
			{
				return;
			}
			if (precision != 0)
			{
				for (std::size_t i = 0; i < N; ++i)
				{
					mpfr_clear(in[i]);
				}
				mpfr_clear(out);
			}
			for (std::size_t i = 0; i < N; ++i)
			{
				mpfr_init2(in[i], wanted);
			}
			mpfr_init2(out, wanted);
			precision = wanted;
		}
	};

	template <std::size_t N> inline mpfr_scratch<N> & thread_scratch()
	{
		thread_local mpfr_scratch<N> scratch;
		return scratch;
	}

	// Standard MPFR unary signature shared by exp, log, the trig family, cbrt, sqrt, and gamma.
	using mpfr_unary_op = int (*)(mpfr_t, const mpfr_t, mpfr_rnd_t);

	// make_*_ref return a reference callable T(const std::array<T, N>&, precision, rounding). They
	// are the one place the differing MPFR signatures are handled.
	template <typename T> inline auto make_unary_ref(mpfr_unary_op op)
	{
		return [op](const std::array<T, 1> & inputs, mpfr_prec_t precision, mpfr_rnd_t rounding) -> T {
			mpfr_scratch<1> & scratch = thread_scratch<1>();
			scratch.ensure(precision);
			mpfr_set_scalar(scratch.in[0], inputs[0]);
			op(scratch.out, scratch.in[0], rounding);
			return mpfr_get_scalar<T>(scratch.out, rounding);
		};
	}

	template <typename T> inline auto make_lgamma_ref()
	{
		return [](const std::array<T, 1> & inputs, mpfr_prec_t precision, mpfr_rnd_t rounding) -> T {
			mpfr_scratch<1> & scratch = thread_scratch<1>();
			scratch.ensure(precision);
			mpfr_set_scalar(scratch.in[0], inputs[0]);
			int sign = 0;
			mpfr_lgamma(scratch.out, &sign, scratch.in[0], rounding);
			return mpfr_get_scalar<T>(scratch.out, rounding);
		};
	}

	template <typename T> inline auto make_pow_ref()
	{
		return [](const std::array<T, 2> & inputs, mpfr_prec_t precision, mpfr_rnd_t rounding) -> T {
			mpfr_scratch<2> & scratch = thread_scratch<2>();
			scratch.ensure(precision);
			mpfr_set_scalar(scratch.in[0], inputs[0]);
			mpfr_set_scalar(scratch.in[1], inputs[1]);
			mpfr_pow(scratch.out, scratch.in[0], scratch.in[1], rounding);
			return mpfr_get_scalar<T>(scratch.out, rounding);
		};
	}

	template <typename T> inline auto make_fma_ref()
	{
		return [](const std::array<T, 3> & inputs, mpfr_prec_t precision, mpfr_rnd_t rounding) -> T {
			mpfr_scratch<3> & scratch = thread_scratch<3>();
			scratch.ensure(precision);
			mpfr_set_scalar(scratch.in[0], inputs[0]);
			mpfr_set_scalar(scratch.in[1], inputs[1]);
			mpfr_set_scalar(scratch.in[2], inputs[2]);
			mpfr_fma(scratch.out, scratch.in[0], scratch.in[1], scratch.in[2], rounding);
			return mpfr_get_scalar<T>(scratch.out, rounding);
		};
	}

	template <typename T> inline mpfr_prec_t default_precision()
	{
		// Well above the table-maker's-dilemma hardness for these functions at each width, so the
		// MPFR value rounds to the correctly-rounded result of the target type.
		if constexpr (std::is_same_v<T, float>)
		{
			return 96;
		}
		return 256;
	}

	// ULP histogram buckets for finite scored cases: index 0..4 is an exact ULP distance, index 5 is
	// every distance of 5 or more (out of the 4 ULP contract). The buckets sum to finite_count.
	inline constexpr std::size_t ulp_bucket_count = 6;

	inline std::size_t ulp_bucket(std::uint64_t distance)
	{
		return distance < ulp_bucket_count ? static_cast<std::size_t>(distance) : ulp_bucket_count - 1;
	}

	template <typename T, std::size_t N> struct campaign_summary
	{
		std::size_t case_count		   = 0;
		std::size_t failure_count	   = 0;
		std::size_t above_target_count = 0;
		std::uint64_t finite_count	   = 0;
		std::uint64_t max_observed_ulp = 0;
		long double ulp_sum			   = 0.0L;
		std::array<std::uint64_t, ulp_bucket_count> ulp_histogram{};
		std::array<T, N> worst_inputs{};
		T worst_actual{};
		T worst_expected{};
	};

	template <typename T, std::size_t N> inline void merge_summary(campaign_summary<T, N> & into, const campaign_summary<T, N> & from)
	{
		into.case_count += from.case_count;
		into.failure_count += from.failure_count;
		into.above_target_count += from.above_target_count;
		into.finite_count += from.finite_count;
		into.ulp_sum += from.ulp_sum;
		for (std::size_t i = 0; i < ulp_bucket_count; ++i)
		{
			into.ulp_histogram[i] += from.ulp_histogram[i];
		}
		if (from.max_observed_ulp >= into.max_observed_ulp)
		{
			into.max_observed_ulp = from.max_observed_ulp;
			into.worst_inputs	  = from.worst_inputs;
			into.worst_actual	  = from.worst_actual;
			into.worst_expected	  = from.worst_expected;
		}
	}

	// Optional sink that writes every input whose ULP distance reaches a threshold to a text file,
	// one case per line in full round-trippable decimal, as the inputs are found. The file write is
	// mutex-guarded so it is safe from the worker threads, and the line is formatted outside the lock.
	// With more than one thread the lines land in the order the workers reach them; pass --threads=1
	// for a single deterministic stream.
	class threshold_recorder
	{
	public:
		threshold_recorder(const std::string & output_path, std::uint64_t threshold) : out_(output_path, std::ios::trunc), threshold_(threshold) {}

		bool active() const
		{
			return out_.is_open();
		}

		template <typename T, std::size_t N> void maybe_record(std::uint64_t distance, const std::array<T, N> & inputs)
		{
			if (distance < threshold_)
			{
				return;
			}
			std::ostringstream line;
			line << std::setprecision(std::numeric_limits<T>::max_digits10);
			for (std::size_t i = 0; i < N; ++i)
			{
				if (i != 0)
				{
					line << ' ';
				}
				line << inputs[i];
			}
			const std::string text = line.str();
			const std::lock_guard<std::mutex> lock(mutex_);
			out_ << text << '\n';
		}

	private:
		std::ofstream out_;
		std::mutex mutex_;
		std::uint64_t threshold_;
	};

	// One case: run the kernel and the references, classify, and fold the result into the summary.
	// Exceptional and zero results are matched against libm or MPFR rather than scored as a ULP
	// distance, exactly as the per-arity evaluators in the shared harness do.
	template <typename T, std::size_t N, typename KernelFn, typename RefFn, typename StdFn>
	inline void evaluate_case(const std::array<T, N> & inputs,
							  KernelFn kernel,
							  RefFn reference,
							  StdFn std_reference,
							  mpfr_prec_t precision,
							  mpfr_rnd_t rounding,
							  std::uint64_t max_ulp,
							  campaign_summary<T, N> & summary,
							  threshold_recorder * recorder)
	{
		++summary.case_count;
		const T actual = kernel(inputs);

		// Exceptional kernel result: cross-check libm first, which is far cheaper than MPFR, and only
		// pay for the correctly-rounded MPFR value when libm disagrees. This skips the MPFR call over
		// the large overflow, underflow, and out-of-domain regions where the result is inf, NaN, or
		// zero and matches libm, which is most of the input space for exp, log, sqrt, and inverse trig.
		if (is_exceptional_or_zero_result(actual))
		{
			const T std_expected = std_reference(inputs);
			if (exceptional_or_zero_match(actual, std_expected))
			{
				return;
			}
			const T mpfr_expected = reference(inputs, precision, rounding);
			if (!exceptional_or_zero_match(actual, mpfr_expected))
			{
				++summary.failure_count;
			}
			return;
		}

		// Finite kernel result: the correctly-rounded MPFR value is needed for the ULP distance.
		const T mpfr_expected = reference(inputs, precision, rounding);
		if (is_exceptional_or_zero_result(mpfr_expected))
		{
			const T std_expected = std_reference(inputs);
			if (!exceptional_or_zero_match(actual, std_expected) && !exceptional_or_zero_match(actual, mpfr_expected))
			{
				++summary.failure_count;
			}
			return;
		}

		std::uint64_t distance = 0;
		std::string notes;
		const bool pass = oracle_match(actual, mpfr_expected, max_ulp, distance, notes);
		if (recorder != nullptr)
		{
			recorder->maybe_record(distance, inputs);
		}
		summary.ulp_sum += static_cast<long double>(distance);
		++summary.ulp_histogram[ulp_bucket(distance)];
		++summary.finite_count;
		if (distance >= summary.max_observed_ulp)
		{
			summary.max_observed_ulp = distance;
			summary.worst_inputs	 = inputs;
			summary.worst_actual	 = actual;
			summary.worst_expected	 = mpfr_expected;
		}
		if (!pass)
		{
			++summary.failure_count;
		} else if (distance > 0)
		{
			++summary.above_target_count;
		}
	}

	template <typename T, std::size_t N> struct campaign_report
	{
		std::string function_name;
		std::string input_type;
		std::string implementation;
		std::string oracle;
		std::string corpus;
		std::string rounding_modes;
		std::size_t case_count		   = 0;
		std::size_t hard_failure_count = 0;
		std::size_t above_target_count = 0;
		std::uint64_t finite_count	   = 0;
		std::uint64_t max_observed_ulp = 0;
		double average_ulp			   = 0.0;
		std::array<std::uint64_t, ulp_bucket_count> ulp_histogram{};
		std::string worst_inputs_bits;
		std::string worst_actual_bits;
		std::string worst_expected_bits;
		unsigned long oracle_precision = 0;
		std::string compiler;
		std::string platform;
		std::string optimization_mode;
		std::uint64_t seed		 = 0;
		std::uint64_t elapsed_ms = 0;
		std::string timestamp;
	};

	template <typename T, std::size_t N> inline std::string join_input_bits(const std::array<T, N> & inputs)
	{
		std::string joined;
		for (std::size_t i = 0; i < N; ++i)
		{
			if (i != 0)
			{
				joined += ",";
			}
			joined += bits_hex(inputs[i]);
		}
		return joined;
	}

	template <typename T, std::size_t N> inline void write_campaign_summary_json(const std::string & output_path, const campaign_report<T, N> & report)
	{
		std::ofstream out(output_path, std::ios::trunc);
		out << "{\n";
		out << "  \"function_name\": \"" << json_escape(report.function_name) << "\",\n";
		out << "  \"input_type\": \"" << json_escape(report.input_type) << "\",\n";
		out << "  \"implementation\": \"" << json_escape(report.implementation) << "\",\n";
		out << "  \"arity\": " << N << ",\n";
		out << "  \"oracle\": \"" << json_escape(report.oracle) << "\",\n";
		out << "  \"corpus\": \"" << json_escape(report.corpus) << "\",\n";
		out << "  \"rounding_modes\": \"" << json_escape(report.rounding_modes) << "\",\n";
		out << "  \"case_count\": " << report.case_count << ",\n";
		out << "  \"hard_failure_count\": " << report.hard_failure_count << ",\n";
		out << "  \"above_target_count\": " << report.above_target_count << ",\n";
		out << "  \"finite_count\": " << report.finite_count << ",\n";
		out << "  \"max_observed_ulp\": " << report.max_observed_ulp << ",\n";
		out << "  \"average_ulp\": " << report.average_ulp << ",\n";
		out << "  \"ulp_histogram\": [";
		for (std::size_t i = 0; i < ulp_bucket_count; ++i)
		{
			if (i != 0)
			{
				out << ", ";
			}
			out << report.ulp_histogram[i];
		}
		out << "],\n";
		out << "  \"worst_input_bits\": \"" << report.worst_inputs_bits << "\",\n";
		out << "  \"worst_actual_bits\": \"" << report.worst_actual_bits << "\",\n";
		out << "  \"worst_expected_bits\": \"" << report.worst_expected_bits << "\",\n";
		out << "  \"oracle_precision\": " << report.oracle_precision << ",\n";
		out << "  \"compiler\": \"" << json_escape(report.compiler) << "\",\n";
		out << "  \"platform\": \"" << json_escape(report.platform) << "\",\n";
		out << "  \"optimization_mode\": \"" << json_escape(report.optimization_mode) << "\",\n";
		out << "  \"seed\": " << report.seed << ",\n";
		out << "  \"elapsed_ms\": " << report.elapsed_ms << ",\n";
		out << "  \"timestamp\": \"" << json_escape(report.timestamp) << "\"\n";
		out << "}\n";
	}

	inline const char * current_mode_token(int mode)
	{
		switch (mode)
		{
		case FE_TONEAREST : return "nearest";
		case FE_UPWARD	  : return "upward";
		case FE_DOWNWARD  : return "downward";
		case FE_TOWARDZERO: return "towardzero";
		default			  : return "unknown";
		}
	}

	inline std::string rounding_modes_label(const std::vector<int> & modes)
	{
		std::string label;
		for (std::size_t i = 0; i < modes.size(); ++i)
		{
			if (i != 0)
			{
				label += ",";
			}
			label += current_mode_token(modes[i]);
		}
		return label;
	}

	// Per-function domain used when sampling a coordinate. The whole-range bit-pattern draws always
	// run too, so this only steers where the targeted sampling spends its budget.
	struct domain
	{
		double lo		   = -std::numeric_limits<double>::infinity();
		double hi		   = std::numeric_limits<double>::infinity();
		bool log_magnitude = false; // sample across exponents rather than uniformly over [lo, hi]
		int min_exp		   = -1074;
		int max_exp		   = 1023;
	};

	template <typename T> inline std::vector<T> structured_inputs()
	{
		const T inf	   = std::numeric_limits<T>::infinity();
		const T nan	   = std::numeric_limits<T>::quiet_NaN();
		const T max	   = std::numeric_limits<T>::max();
		const T min	   = std::numeric_limits<T>::min();
		const T denorm = std::numeric_limits<T>::denorm_min();
		const T pi	   = static_cast<T>(3.141592653589793238462643383279502884L);

		return {
			static_cast<T>(0),
			-static_cast<T>(0),
			static_cast<T>(1),
			-static_cast<T>(1),
			static_cast<T>(0.5),
			-static_cast<T>(0.5),
			static_cast<T>(2),
			-static_cast<T>(2),
			static_cast<T>(0.25),
			-static_cast<T>(0.25),
			static_cast<T>(10),
			-static_cast<T>(10),
			pi,
			-pi,
			pi / static_cast<T>(2),
			pi / static_cast<T>(4),
			pi / static_cast<T>(6),
			max,
			-max,
			min,
			-min,
			denorm,
			-denorm,
			inf,
			-inf,
			nan,
		};
	}

	// Function shape. The dispatch tags each function with one, and unary_structured_seed reads it to
	// choose the hard-case inputs and the over/underflow boundary search. A new function takes the
	// nearest family.
	enum class function_family
	{
		generic,
		exp,
		logarithm,
		trig,
		inverse_trig,
		power,
		gamma,
	};

	// Which implementation of a function to measure, chosen with --impl. The generic kernel is the
	// default and the thing ccmath owns. The public path lowers to libm under FE_TONEAREST on clang and
	// gcc, so it matches libm in round-to-nearest and shows its own behaviour only in the directed modes
	// and at compile time. libm is the system math library reached through the standard headers.
	enum class impl_kind
	{
		generic,
		public_api,
		libm,
	};

	inline impl_kind parse_impl(int argc, char ** argv)
	{
		const auto raw = option_value(argc, argv, "--impl=");
		if (!raw.has_value() || *raw == "gen" || *raw == "generic")
		{
			return impl_kind::generic;
		}
		if (*raw == "public")
		{
			return impl_kind::public_api;
		}
		if (*raw == "libm" || *raw == "std")
		{
			return impl_kind::libm;
		}
		std::cerr << "unknown --impl=" << *raw << " (expected gen, public, or libm), using gen\n";
		return impl_kind::generic;
	}

	inline std::string_view impl_token(impl_kind impl)
	{
		switch (impl)
		{
		case impl_kind::generic	  : return "gen";
		case impl_kind::public_api: return "public";
		case impl_kind::libm	  : return "libm";
		}
		return "gen";
	}

	// Deterministic hard-case inputs beyond the common structured edges, chosen per family: the branch
	// points and cancellation regions where faithful-but-not-correctly-rounded results tend to hide.
	// These are seeded into the default corpus so every sweep measures them regardless of the random
	// draw. The boundary inputs (overflow and underflow) are found separately by unary_boundary_cases.
	template <typename T> inline std::vector<T> unary_extra_inputs(function_family family)
	{
		std::vector<T> values;
		const auto add		= [&](double value) { values.push_back(static_cast<T>(value)); };
		const auto add_pow2 = [&](int lo, int hi) {
			for (int k = lo; k <= hi; ++k)
			{
				add(std::ldexp(1.0, k));
				add(-std::ldexp(1.0, k));
			}
		};
		const T one = static_cast<T>(1);

		switch (family)
		{
		case function_family::exp:
			for (int n = -16; n <= 16; ++n)
			{
				add(static_cast<double>(n));
			} // integers, exp2 lands on a power of two
			add_pow2(-28, 28);
			add(std::log(2.0));
			add(std::log(10.0));
			break;
		case function_family::logarithm:
		{
			// The dyadic neighbourhood of 1, where log and log1p cancel hardest.
			T up   = one;
			T down = one;
			for (int i = 0; i < 8; ++i)
			{
				up	 = std::nextafter(up, static_cast<T>(2));
				down = std::nextafter(down, static_cast<T>(0));
				values.push_back(up);
				values.push_back(down);
			}
			add_pow2(-28, 28); // exact integer results for log2
			add(std::exp(1.0));
			break;
		}
		case function_family::trig:
			// Multiples of pi/2 from small to large, the worst case for argument reduction, up through
			// the Payne-Hanek cancellation region at large magnitudes.
			for (int k = 1; k <= 64; ++k)
			{
				add(static_cast<double>(k) * 1.57079632679489661923);
				add(-static_cast<double>(k) * 1.57079632679489661923);
			}
			for (int e : { 10, 16, 20, 23, 30, 40, 50, 60, 80, 100 })
			{
				add(std::ldexp(1.57079632679489661923, e));
				add(std::ldexp(1.0, e));
			}
			break;
		case function_family::inverse_trig:
		{
			const T inside	= std::nextafter(one, static_cast<T>(0)); // just inside the +/-1 domain edge
			const T outside = std::nextafter(one, static_cast<T>(2)); // just outside, asin and acos go NaN
			values.push_back(inside);
			values.push_back(-inside);
			values.push_back(outside);
			values.push_back(-outside);
			add(0.5);
			add(-0.5);
			add(0.70710678118654752440);
			add(-0.70710678118654752440);
			add(0.86602540378443864676);
			break;
		}
		case function_family::power:
			for (int n = 1; n <= 16; ++n)
			{
				add(static_cast<double>(n) * static_cast<double>(n));
			} // perfect squares
			for (int n = -5; n <= 5; ++n)
			{
				add(static_cast<double>(n) * static_cast<double>(n) * static_cast<double>(n));
			} // perfect cubes, cbrt of negatives
			add_pow2(-28, 28);
			break;
		case function_family::gamma:
			for (int n = 0; n <= 20; ++n)
			{
				add(-static_cast<double>(n));
			} // poles at zero and the negative integers
			for (int n = 0; n <= 10; ++n)
			{
				const T pole = static_cast<T>(-n); // step a few ULP off each pole, where the dynamic range is largest
				values.push_back(std::nextafter(pole, static_cast<T>(1)));
				values.push_back(std::nextafter(pole, static_cast<T>(-100)));
				add(-static_cast<double>(n) + 1.0e-3);
				add(-static_cast<double>(n) - 1.0e-3);
			}
			for (int n = 1; n <= 15; ++n)
			{
				add(static_cast<double>(n));
			} // tgamma(n) is (n-1)!
			for (int n = 0; n <= 10; ++n)
			{
				add(static_cast<double>(n) + 0.5);
			} // half-integers
			break;
		case function_family::generic: add_pow2(-16, 16); break;
		}
		return values;
	}

	// The IEEE/C special-value matrix for a two-argument function such as pow: the cross product of the
	// special and boundary values for each argument. A random bit-pattern draw almost never makes both
	// arguments special at once, so these enumerated corners (pow(x, +/-0) is 1, pow(+/-1, inf) is 1,
	// pow(negative, non-integer) is NaN, pow(+/-0, negative odd) is +/-inf, and the rest) are measured
	// only because they are seeded here.
	template <typename T> inline std::vector<std::array<T, 2>> pow_special_matrix()
	{
		const T inf = std::numeric_limits<T>::infinity();
		const T nan = std::numeric_limits<T>::quiet_NaN();
		const T one = static_cast<T>(1);

		const std::vector<T> bases	   = { static_cast<T>(0),
										   -static_cast<T>(0),
										   one,
										   -one,
										   static_cast<T>(2),
										   -static_cast<T>(2),
										   static_cast<T>(0.5),
										   -static_cast<T>(0.5),
										   static_cast<T>(3),
										   -static_cast<T>(3),
										   std::nextafter(one, static_cast<T>(2)),
										   std::nextafter(one, static_cast<T>(0)),
										   static_cast<T>(1.0e30),
										   static_cast<T>(-1.0e30),
										   static_cast<T>(1.0e-30),
										   inf,
										   -inf,
										   nan };
		const std::vector<T> exponents = { static_cast<T>(0),
										   -static_cast<T>(0),
										   one,
										   -one,
										   static_cast<T>(2),
										   -static_cast<T>(2),
										   static_cast<T>(3),
										   -static_cast<T>(3),
										   static_cast<T>(0.5),
										   -static_cast<T>(0.5),
										   static_cast<T>(2.5),
										   -static_cast<T>(2.5),
										   static_cast<T>(100),
										   -static_cast<T>(100),
										   static_cast<T>(1.0e30),
										   -static_cast<T>(1.0e30),
										   inf,
										   -inf,
										   nan };

		std::vector<std::array<T, 2>> cases;
		cases.reserve(bases.size() * exponents.size());
		for (const T base : bases)
		{
			for (const T exponent : exponents)
			{
				cases.push_back({ base, exponent });
			}
		}
		return cases;
	}

	// Pin the exact over/underflow boundary inputs by bisecting the standard reference for the
	// finite-to-special transition: the last input with a finite result and the first that overflows to
	// infinity or underflows to zero. These adjacent floats are where directed-rounding overflow handling
	// is decided, so they belong in every corpus. Only the families with a clean monotone boundary run a
	// search; the gate on the endpoint result keeps it from firing where there is no transition.
	template <typename T, typename StdScalar> inline std::vector<std::array<T, 1>> unary_boundary_cases(StdScalar std_fn, function_family family)
	{
		std::vector<std::array<T, 1>> out;
		const T anchor = static_cast<T>(1); // finite for every function here

		const auto bisect = [&](T special_pt, auto is_past) {
			T finite_pt = anchor;
			for (int i = 0; i < 5000; ++i)
			{
				const T mid = finite_pt + (special_pt - finite_pt) / static_cast<T>(2);
				if (mid == finite_pt || mid == special_pt)
				{
					break;
				}
				if (is_past(std_fn(mid)))
				{
					special_pt = mid;
				} else
				{
					finite_pt = mid;
				}
			}
			out.push_back({ finite_pt });
			out.push_back({ special_pt });
		};

		const bool has_overflow	 = family == function_family::exp || family == function_family::gamma;
		const bool has_underflow = family == function_family::exp;
		const auto is_inf		 = [](T value) { return std::isinf(value); };
		const auto is_zero		 = [](T value) { return value == static_cast<T>(0); };

		if (has_overflow && is_inf(std_fn(std::numeric_limits<T>::max())))
		{
			bisect(std::numeric_limits<T>::max(), is_inf);
		}
		if (has_underflow && is_zero(std_fn(std::numeric_limits<T>::lowest())))
		{
			bisect(std::numeric_limits<T>::lowest(), is_zero);
		}
		return out;
	}

	// The full per-function deterministic seed for the unary case: family hard cases plus the boundary
	// inputs, returned as arity-1 cases for the corpus.
	template <typename T, typename StdScalar> inline std::vector<std::array<T, 1>> unary_structured_seed(StdScalar std_fn, function_family family)
	{
		std::vector<std::array<T, 1>> out;
		for (const T value : unary_extra_inputs<T>(family))
		{
			out.push_back({ value });
		}
		const auto boundary = unary_boundary_cases<T>(std_fn, family);
		out.insert(out.end(), boundary.begin(), boundary.end());
		return out;
	}

	inline std::size_t sampled_count(campaign_mode mode)
	{
		switch (mode)
		{
		case campaign_mode::quick	: return 500000;
		case campaign_mode::extended: return 5000000;
		case campaign_mode::full	:
		case campaign_mode::release : return 50000000;
		}
		return 500000;
	}

	// Unary corpus: structured edges, a per-function domain draw, and broad bit patterns.
	template <typename T> inline std::vector<std::array<T, 1>> build_unary_corpus(const domain & dom, std::uint64_t seed, std::size_t count)
	{
		std::vector<std::array<T, 1>> cases;
		for (const T value : structured_inputs<T>())
		{
			cases.push_back({ value });
		}

		std::mt19937_64 rng(seed);
		if (dom.log_magnitude)
		{
			std::uniform_int_distribution<int> exp_dist(dom.min_exp, dom.max_exp);
			std::uniform_real_distribution<double> mant(1.0, 2.0);
			const bool both_signs = dom.lo < 0.0;
			for (std::size_t i = 0; i < count; ++i)
			{
				double value = std::ldexp(mant(rng), exp_dist(rng));
				if (both_signs && (rng() & 1U) != 0U)
				{
					value = -value;
				}
				if (value >= dom.lo && value <= dom.hi)
				{
					cases.push_back({ static_cast<T>(value) });
				}
			}
		} else
		{
			const double lo = std::isfinite(dom.lo) ? dom.lo : -1.0e6;
			const double hi = std::isfinite(dom.hi) ? dom.hi : 1.0e6;
			std::uniform_real_distribution<double> uniform(lo, hi);
			for (std::size_t i = 0; i < count; ++i)
			{
				cases.push_back({ static_cast<T>(uniform(rng)) });
			}
		}

		std::mt19937_64 bits(seed ^ 0x9E3779B97F4A7C15ULL);
		const std::size_t bit_pattern_count = count / 4 + 1;
		for (std::size_t i = 0; i < bit_pattern_count; ++i)
		{
			if constexpr (std::is_same_v<T, float>)
			{
				cases.push_back({ ccm::support::bit_cast<float>(static_cast<std::uint32_t>(bits())) });
			} else
			{
				cases.push_back({ ccm::support::bit_cast<double>(bits()) });
			}
		}
		return cases;
	}

	// pow corpus: the targeted finite-result band and hard-case families plus broad bit patterns,
	// reusing the pow case generators from oracle_campaign_common.hpp.
	template <typename T> inline std::vector<std::array<T, 2>> build_pow_corpus(std::uint64_t seed, std::size_t count)
	{
		// Generate in chunks so the heavier pow_case intermediate (it carries a provenance string) is
		// never fully materialized at once. Only the compact array<T, 2> corpus is kept, which keeps
		// the per-case cost near 8 bytes and lets a very large sample fit in memory.
		const std::size_t random_count	 = count / 4 + 1;
		constexpr std::size_t chunk_size = std::size_t{ 1 } << 22;

		std::vector<std::array<T, 2>> cases;
		cases.reserve(count + random_count);
		std::vector<pow_case<T>> chunk;

		for (std::size_t done = 0; done < count; done += chunk_size)
		{
			const std::size_t n = std::min(chunk_size, count - done);
			chunk.clear();
			add_targeted_random_cases(chunk, seed + done, n, "pow targeted finite band");
			for (const auto & test_case : chunk)
			{
				cases.push_back({ test_case.base, test_case.exponent });
			}
		}
		for (std::size_t done = 0; done < random_count; done += chunk_size)
		{
			const std::size_t n = std::min(chunk_size, random_count - done);
			chunk.clear();
			add_random_cases(chunk, (seed ^ 0x9E3779B97F4A7C15ULL) + done, n, "pow whole-range bit pattern");
			for (const auto & test_case : chunk)
			{
				cases.push_back({ test_case.base, test_case.exponent });
			}
		}
		return cases;
	}

	// True when value sits inside the closed domain [lo, hi]. NaN is never in a numeric region, and an
	// infinity counts only when the matching bound is infinite, which keeps structured edges sensible.
	template <typename T> inline bool in_domain(const domain & dom, T value)
	{
		if (std::isnan(value))
		{
			return false;
		}
		const double as_double = static_cast<double>(value);
		return as_double >= dom.lo && as_double <= dom.hi;
	}

	// Draw one value from a single axis. log_magnitude spreads draws evenly across exponents (the right
	// shape for functions whose error is per-binade), otherwise the draw is uniform over [lo, hi]. The
	// log draw retries a few times to land inside [lo, hi] before clamping, so a tight clip stays honest
	// without risking an unbounded loop.
	template <typename T> inline T draw_in_domain(const domain & dom, std::mt19937_64 & rng)
	{
		if (dom.log_magnitude)
		{
			std::uniform_int_distribution<int> exp_dist(dom.min_exp, dom.max_exp);
			std::uniform_real_distribution<double> mant(1.0, 2.0);
			const bool both_signs = dom.lo < 0.0;
			for (int attempt = 0; attempt < 8; ++attempt)
			{
				double value = std::ldexp(mant(rng), exp_dist(rng));
				if (both_signs && (rng() & 1U) != 0U)
				{
					value = -value;
				}
				if (value >= dom.lo && value <= dom.hi)
				{
					return static_cast<T>(value);
				}
			}
			return static_cast<T>(std::min(std::max(0.0, dom.lo), dom.hi));
		}
		const double lo = std::isfinite(dom.lo) ? dom.lo : -1.0e6;
		const double hi = std::isfinite(dom.hi) ? dom.hi : 1.0e6;
		std::uniform_real_distribution<double> uniform(lo, hi);
		return static_cast<T>(uniform(rng));
	}

	// Generic arity-N corpus confined to per-argument domains. Each coordinate draws independently from
	// its own domain with its own stream, so a region of the joint space is sampled directly. When not
	// confined and N == 1 the in-region structured edges are prepended; a confined sweep is region draws
	// only, which is what you want when aiming a deep sample at one band.
	template <typename T, std::size_t N>
	inline std::vector<std::array<T, N>> build_region_corpus(const std::array<domain, N> & domains, std::uint64_t seed, std::size_t count, bool confine)
	{
		std::vector<std::array<T, N>> cases;
		cases.reserve(count + 32);
		if constexpr (N == 1)
		{
			if (!confine)
			{
				for (const T value : structured_inputs<T>())
				{
					if (in_domain(domains[0], value))
					{
						cases.push_back({ value });
					}
				}
			}
		}

		std::array<std::mt19937_64, N> rng;
		for (std::size_t axis = 0; axis < N; ++axis)
		{
			rng[axis] = std::mt19937_64(seed + 0x9E3779B97F4A7C15ULL * (axis + 1));
		}
		for (std::size_t i = 0; i < count; ++i)
		{
			std::array<T, N> input{};
			for (std::size_t axis = 0; axis < N; ++axis)
			{
				input[axis] = draw_in_domain<T>(domains[axis], rng[axis]);
			}
			cases.push_back(input);
		}
		return cases;
	}

	// Bit pattern of a scalar as an unsigned integer, used as a total-order key for deduplication.
	// Keying on bits keeps -0.0 distinct from +0.0 and keeps distinct NaN payloads distinct.
	template <typename T> inline auto canonical_bits(T value)
	{
		if constexpr (sizeof(T) == 4)
		{
			return ccm::support::bit_cast<std::uint32_t>(value);
		} else
		{
			return ccm::support::bit_cast<std::uint64_t>(value);
		}
	}

	// Drop exact-bit duplicate cases from a sampled corpus so a random sweep never measures the same
	// input twice. Sorting by bit pattern and removing adjacent equals happens in place, so it adds no
	// memory beyond the corpus itself and scales to the largest samples. Corpus order does not affect
	// the measurement, so the reordering is harmless. Returns how many repeats were removed.
	template <typename T, std::size_t N> inline std::size_t deduplicate_cases(std::vector<std::array<T, N>> & cases)
	{
		const auto less = [](const std::array<T, N> & a, const std::array<T, N> & b) {
			for (std::size_t i = 0; i < N; ++i)
			{
				const auto ka = canonical_bits(a[i]);
				const auto kb = canonical_bits(b[i]);
				if (ka != kb)
				{
					return ka < kb;
				}
			}
			return false;
		};
		const auto equal = [](const std::array<T, N> & a, const std::array<T, N> & b) {
			for (std::size_t i = 0; i < N; ++i)
			{
				if (canonical_bits(a[i]) != canonical_bits(b[i]))
				{
					return false;
				}
			}
			return true;
		};
		const std::size_t before = cases.size();
		std::sort(cases.begin(), cases.end(), less);
		cases.erase(std::unique(cases.begin(), cases.end(), equal), cases.end());
		return before - cases.size();
	}

	// Read a corpus of explicit cases from a text file, one case per line, N whitespace-separated values
	// in decimal or 0x hex. This is the same shape --record-output writes, so a dump of hard cases from
	// one run replays directly, and external worst-case tables (Lefevre, CORE-MATH, RLIBM) drop straight
	// in. Malformed lines are skipped with a count on stderr.
	template <typename T, std::size_t N> inline std::vector<std::array<T, N>> read_input_file(const std::string & path)
	{
		std::vector<std::array<T, N>> cases;
		std::ifstream in(path);
		if (!in)
		{
			std::cerr << "input file not found: " << path << '\n';
			return cases;
		}
		std::string line;
		std::size_t skipped = 0;
		while (std::getline(in, line))
		{
			std::istringstream stream(line);
			std::array<T, N> values{};
			std::size_t parsed = 0;
			double token	   = 0.0;
			while (parsed < N && (stream >> token))
			{
				values[parsed++] = static_cast<T>(token);
			}
			if (parsed == N)
			{
				cases.push_back(values);
			} else if (!line.empty())
			{
				++skipped;
			}
		}
		if (skipped != 0)
		{
			std::cerr << "skipped " << skipped << " malformed line(s) in " << path << '\n';
		}
		return cases;
	}

	// Widen coverage around a set of seeded cases by adding each coordinate's two nextafter neighbours.
	// The correctly-rounded result of a function near a seed is most likely to sit just over a rounding
	// boundary at an adjacent float, so probing neighbours of the known-interesting inputs is a cheap way
	// to catch hard-to-round cases the random draw misses.
	template <typename T, std::size_t N> inline void add_neighbor_probes(std::vector<std::array<T, N>> & cases)
	{
		const std::size_t original = cases.size();
		cases.reserve(original * (2 * N + 1));
		const T up	 = std::numeric_limits<T>::infinity();
		const T down = -std::numeric_limits<T>::infinity();
		for (std::size_t i = 0; i < original; ++i)
		{
			const std::array<T, N> base = cases[i];
			for (std::size_t axis = 0; axis < N; ++axis)
			{
				std::array<T, N> above = base;
				std::array<T, N> below = base;
				above[axis]			   = std::nextafter(base[axis], up);
				below[axis]			   = std::nextafter(base[axis], down);
				cases.push_back(above);
				cases.push_back(below);
			}
		}
	}

	// Read one axis domain from the command line, falling back to the per-function default. Each field
	// has an unindexed flag that applies to every axis and an indexed flag (--lo0, --hi1, ...) that wins
	// for that axis, so a unary sweep stays simple while pow can clip each argument independently.
	inline domain parse_axis_domain(int argc, char ** argv, std::size_t index, const domain & fallback)
	{
		domain dom			  = fallback;
		const std::string idx = std::to_string(index);
		const auto field	  = [&](const std::string & base) -> std::optional<std::string> {
			if (auto indexed = option_value(argc, argv, base + idx + "="))
			{
				return indexed;
			}
			return option_value(argc, argv, base + "=");
		};
		if (const auto value = field("--lo"))
		{
			dom.lo = std::stod(*value);
		}
		if (const auto value = field("--hi"))
		{
			dom.hi = std::stod(*value);
		}
		if (const auto value = field("--min-exp"))
		{
			dom.min_exp = std::stoi(*value);
		}
		if (const auto value = field("--max-exp"))
		{
			dom.max_exp = std::stoi(*value);
		}
		if (const auto value = field("--scale"))
		{
			dom.log_magnitude = (*value == "log");
		}
		return dom;
	}

	// True when any axis-domain flag is present, which switches the corpus from the per-function default
	// builder over to the confined region builder.
	inline bool domain_flags_present(int argc, char ** argv, std::size_t arity)
	{
		static const char * const bases[] = { "--lo", "--hi", "--min-exp", "--max-exp", "--scale" };
		for (const char * base : bases)
		{
			if (option_value(argc, argv, std::string(base) + "="))
			{
				return true;
			}
			for (std::size_t index = 0; index < arity; ++index)
			{
				if (option_value(argc, argv, std::string(base) + std::to_string(index) + "="))
				{
					return true;
				}
			}
		}
		return false;
	}

	// Half-open bit-pattern interval [begin, end) over the 2^32 binary32 patterns, held in 64-bit so the
	// top end (2^32) is representable.
	struct bit_interval
	{
		std::uint64_t begin = 0;
		std::uint64_t end	= 0;
	};

	// The bit-pattern intervals an exhaustive binary32 sweep should cover. An explicit --exhaustive-range
	// wins (decimal or 0x hex, inclusive). Otherwise a finite [lo, hi] on axis 0 is mapped to its
	// covering interval(s): non-negative floats are monotone in their bit pattern, negatives are monotone
	// in reverse, and a range straddling zero becomes one interval per side. With no clip it is the whole
	// space.
	inline std::vector<bit_interval> float_exhaustive_intervals(int argc, char ** argv, const domain & axis0)
	{
		if (const auto raw = option_value(argc, argv, "--exhaustive-range="))
		{
			const std::size_t colon = raw->find(':');
			const std::uint64_t lo	= std::stoull(raw->substr(0, colon), nullptr, 0);
			const std::uint64_t hi	= colon == std::string::npos ? lo : std::stoull(raw->substr(colon + 1), nullptr, 0);
			return { bit_interval{ lo, hi + 1 } };
		}

		const auto to_bits = [](float value) { return static_cast<std::uint64_t>(ccm::support::bit_cast<std::uint32_t>(value)); };
		if (std::isfinite(axis0.lo) && std::isfinite(axis0.hi))
		{
			const float lo = static_cast<float>(axis0.lo);
			const float hi = static_cast<float>(axis0.hi);
			if (lo >= 0.0F)
			{
				return { bit_interval{ to_bits(lo), to_bits(hi) + 1 } };
			}
			if (hi <= 0.0F)
			{
				return { bit_interval{ to_bits(hi), to_bits(lo) + 1 } };
			}
			return { bit_interval{ to_bits(-0.0F), to_bits(lo) + 1 }, bit_interval{ to_bits(0.0F), to_bits(hi) + 1 } };
		}

		return { bit_interval{ 0, std::uint64_t{ 1 } << 32 } };
	}

	inline std::string format_magnitude(std::uint64_t value)
	{
		if (value < 1000)
		{
			return std::to_string(value);
		}
		static const char * const suffix[] = { "", "K", "M", "B", "T" };
		double scaled					   = static_cast<double>(value);
		std::size_t step				   = 0;
		while (scaled >= 1000.0 && step + 1 < std::size(suffix))
		{
			scaled /= 1000.0;
			++step;
		}
		std::ostringstream out;
		out << std::fixed << std::setprecision(scaled < 10.0 ? 2 : (scaled < 100.0 ? 1 : 0)) << scaled << suffix[step];
		return out.str();
	}

	inline std::string format_duration(double seconds)
	{
		std::ostringstream out;
		if (seconds < 90.0)
		{
			out << static_cast<long long>(seconds) << 's';
		} else if (seconds < 5400.0)
		{
			out << std::fixed << std::setprecision(1) << seconds / 60.0 << 'm';
		} else if (seconds < 172800.0)
		{
			out << std::fixed << std::setprecision(1) << seconds / 3600.0 << 'h';
		} else
		{
			out << std::fixed << std::setprecision(1) << seconds / 86400.0 << 'd';
		}
		return out.str();
	}

	// Optional live progress to stderr for the long sweeps. It reads the shared chunk counter (chunks
	// dispatched, so it runs a touch ahead of cases finished) and reports percent, throughput, and an
	// ETA every couple of seconds. Construction starts the thread, destruction stops and joins it, so a
	// pass scopes the ticker around its worker block.
	template <typename CounterT> class progress_ticker
	{
	public:
		progress_ticker(const std::atomic<CounterT> & counter, std::uint64_t chunk, std::uint64_t total, std::string label)
			: counter_(counter),
			  chunk_(chunk),
			  total_(total),
			  label_(std::move(label))
		{
			worker_ = std::thread([this]() { run(); });
		}
		progress_ticker(const progress_ticker &)			 = delete;
		progress_ticker & operator=(const progress_ticker &) = delete;
		~progress_ticker()
		{
			stop_.store(true, std::memory_order_relaxed);
			if (worker_.joinable())
			{
				worker_.join();
			}
		}

	private:
		void run()
		{
			const auto start = std::chrono::steady_clock::now();
			bool printed	 = false;
			while (!stop_.load(std::memory_order_relaxed))
			{
				for (int i = 0; i < 20 && !stop_.load(std::memory_order_relaxed); ++i)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
				if (stop_.load(std::memory_order_relaxed))
				{
					break;
				}
				const std::uint64_t done = std::min(static_cast<std::uint64_t>(counter_.load(std::memory_order_relaxed)) * chunk_, total_);
				const double seconds	 = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
				const double rate		 = seconds > 0.0 ? static_cast<double>(done) / seconds : 0.0;
				const double percent	 = total_ != 0 ? 100.0 * static_cast<double>(done) / static_cast<double>(total_) : 0.0;
				const double eta		 = rate > 0.0 ? static_cast<double>(total_ - done) / rate : 0.0;
				std::cerr << "\rprogress " << label_ << ' ' << std::fixed << std::setprecision(1) << percent << "% " << format_magnitude(done) << '/'
						  << format_magnitude(total_) << ' ' << format_magnitude(static_cast<std::uint64_t>(rate)) << "/s eta " << format_duration(eta)
						  << "    " << std::flush;
				printed = true;
			}
			if (printed)
			{
				std::cerr << '\n';
			}
		}

		const std::atomic<CounterT> & counter_;
		std::uint64_t chunk_;
		std::uint64_t total_;
		std::string label_;
		std::atomic<bool> stop_{ false };
		std::thread worker_;
	};

	// Sampled corpus, evaluated in parallel. The case vector is read-only and shared, and each thread
	// pulls chunks from a shared counter into its own partial summary, the same load-balanced scheme
	// the exhaustive sweep uses. The MPFR default rounding mode is set once per pass and only read by
	// the workers, and each worker sets its own hardware rounding mode.
	template <typename T, std::size_t N, typename KernelFn, typename RefFn, typename StdFn>
	inline campaign_summary<T, N> run_sampled(const std::vector<std::array<T, N>> & cases,
											  const std::vector<int> & rounding_modes,
											  mpfr_prec_t precision,
											  std::uint64_t max_ulp,
											  unsigned int thread_count,
											  KernelFn kernel,
											  RefFn reference,
											  StdFn std_reference,
											  threshold_recorder * recorder,
											  bool show_progress)
	{
		if (thread_count == 0U)
		{
			thread_count = 1U;
		}
		constexpr std::size_t chunk_size = std::size_t{ 1 } << 14;
		const std::size_t total_cases	 = cases.size();

		const mpfr_rnd_t saved_default = mpfr_get_default_rounding_mode();
		campaign_summary<T, N> total;

		for (const int mode : rounding_modes)
		{
			mpfr_set_default_rounding_mode(fe_to_mpfr_rounding(mode));

			std::vector<campaign_summary<T, N>> partials(thread_count);
			std::vector<std::thread> workers;
			workers.reserve(thread_count);
			std::atomic<std::size_t> next_chunk{ 0 };

			std::unique_ptr<progress_ticker<std::size_t>> ticker;
			if (show_progress)
			{
				ticker =
					std::make_unique<progress_ticker<std::size_t>>(next_chunk, chunk_size, total_cases, std::string("sampled ") + current_mode_token(mode));
			}

			for (unsigned int t = 0; t < thread_count; ++t)
			{
				workers.emplace_back([&, t]() {
					ccm::test::ScopedRoundingMode hardware(mode);
					if (!hardware.active())
					{
						return;
					}
					const mpfr_rnd_t rounding		 = mpfr_get_default_rounding_mode();
					campaign_summary<T, N> & summary = partials[t];
					for (;;)
					{
						const std::size_t begin = next_chunk.fetch_add(1, std::memory_order_relaxed) * chunk_size;
						if (begin >= total_cases)
						{
							break;
						}
						const std::size_t end = std::min(begin + chunk_size, total_cases);
						for (std::size_t i = begin; i < end; ++i)
						{
							evaluate_case<T, N>(cases[i], kernel, reference, std_reference, precision, rounding, max_ulp, summary, recorder);
						}
					}
				});
			}
			for (auto & worker : workers)
			{
				worker.join();
			}
			ticker.reset();
			for (const auto & partial : partials)
			{
				merge_summary(total, partial);
			}
		}

		mpfr_set_default_rounding_mode(saved_default);
		return total;
	}

	// Exhaustive binary32 sweep over one or more bit-pattern intervals, only meaningful for a single
	// input. The intervals are flattened into one virtual index space handed out in small chunks from a
	// shared counter, so every core stays busy on the expensive band whether the sweep is the whole
	// 2^32 or a single targeted sub-range.
	template <typename KernelFn, typename RefFn, typename StdFn>
	inline campaign_summary<float, 1> run_exhaustive_float(const std::vector<bit_interval> & intervals,
														   const std::vector<int> & rounding_modes,
														   mpfr_prec_t precision,
														   std::uint64_t max_ulp,
														   unsigned int thread_count,
														   KernelFn kernel,
														   RefFn reference,
														   StdFn std_reference,
														   threshold_recorder * recorder,
														   bool show_progress)
	{
		if (thread_count == 0U)
		{
			thread_count = 1U;
		}
		constexpr std::uint64_t chunk_size = std::uint64_t{ 1 } << 20;

		std::uint64_t total_patterns = 0;
		for (const bit_interval & interval : intervals)
		{
			total_patterns += interval.end - interval.begin;
		}

		// Map a flat index across all intervals back to its bit pattern. The interval count is one or two
		// in practice, so the linear walk is negligible next to an MPFR reference call.
		const auto pattern_at = [&intervals](std::uint64_t index) -> std::uint32_t {
			for (const bit_interval & interval : intervals)
			{
				const std::uint64_t span = interval.end - interval.begin;
				if (index < span)
				{
					return static_cast<std::uint32_t>(interval.begin + index);
				}
				index -= span;
			}
			return 0;
		};

		const mpfr_rnd_t saved_default = mpfr_get_default_rounding_mode();
		campaign_summary<float, 1> total;

		for (const int mode : rounding_modes)
		{
			mpfr_set_default_rounding_mode(fe_to_mpfr_rounding(mode));

			std::vector<campaign_summary<float, 1>> partials(thread_count);
			std::vector<std::thread> workers;
			workers.reserve(thread_count);
			std::atomic<std::uint64_t> next_chunk{ 0 };

			std::unique_ptr<progress_ticker<std::uint64_t>> ticker;
			if (show_progress)
			{
				ticker = std::make_unique<progress_ticker<std::uint64_t>>(
					next_chunk, chunk_size, total_patterns, std::string("exhaustive ") + current_mode_token(mode));
			}

			for (unsigned int t = 0; t < thread_count; ++t)
			{
				workers.emplace_back([&, t]() {
					ccm::test::ScopedRoundingMode hardware(mode);
					if (!hardware.active())
					{
						return;
					}
					const mpfr_rnd_t rounding			 = mpfr_get_default_rounding_mode();
					campaign_summary<float, 1> & summary = partials[t];
					for (;;)
					{
						const std::uint64_t begin = next_chunk.fetch_add(1, std::memory_order_relaxed) * chunk_size;
						if (begin >= total_patterns)
						{
							break;
						}
						const std::uint64_t end = std::min(begin + chunk_size, total_patterns);
						for (std::uint64_t index = begin; index < end; ++index)
						{
							const float input = ccm::support::bit_cast<float>(pattern_at(index));
							if (std::isnan(input))
							{
								continue;
							}
							evaluate_case<float, 1>(
								std::array<float, 1>{ input }, kernel, reference, std_reference, precision, rounding, max_ulp, summary, recorder);
						}
					}
				});
			}
			for (auto & worker : workers)
			{
				worker.join();
			}
			ticker.reset();
			for (const auto & partial : partials)
			{
				merge_summary(total, partial);
			}
		}

		mpfr_set_default_rounding_mode(saved_default);
		return total;
	}

	template <typename T, std::size_t N, typename KernelFn, typename RefFn, typename StdFn, typename CorpusFn>
	inline int run_campaign(int argc,
							char ** argv,
							std::string_view function_name,
							KernelFn kernel,
							RefFn reference,
							StdFn std_reference,
							CorpusFn build_corpus,
							const std::array<domain, N> & default_domains,
							std::vector<std::array<T, N>> structured_seed,
							std::string_view implementation = "gen")
	{
		const auto mode				= parse_mode(option_value(argc, argv, "--mode="));
		const auto rounding_modes	= parse_rounding_modes(argc, argv);
		const auto output_path		= resolve_event_log_path(argc, argv);
		const bool exhaustive_flag	= has_flag(argc, argv, "--exhaustive") || option_value(argc, argv, "--exhaustive-range=").has_value();
		const bool confine			= has_flag(argc, argv, "--confine");
		const bool show_progress	= has_flag(argc, argv, "--progress");
		const bool allow_duplicates = has_flag(argc, argv, "--allow-duplicates");
		const bool probe_neighbors	= has_flag(argc, argv, "--probe-neighbors");
		const auto input_file		= option_value(argc, argv, "--input-file=");
		const std::uint64_t seed =
			parse_option_or<std::uint64_t>(option_value(argc, argv, "--seed="), [](const std::string & value) { return std::stoull(value); }, 0xCCA77811ULL);
		const std::uint64_t max_ulp = parse_option_or<std::uint64_t>(
			option_value(argc, argv, "--max-ulp="), [](const std::string & value) { return static_cast<std::uint64_t>(std::stoull(value)); }, 4);
		const mpfr_prec_t precision = parse_option_or<mpfr_prec_t>(
			option_value(argc, argv, "--oracle-precision="),
			[](const std::string & value) { return static_cast<mpfr_prec_t>(std::stoul(value)); },
			default_precision<T>());
		const std::size_t count = parse_option_or<std::size_t>(
			option_value(argc, argv, "--count="), [](const std::string & value) { return static_cast<std::size_t>(std::stoull(value)); }, sampled_count(mode));
		const unsigned int thread_count = parse_option_or<unsigned int>(
			option_value(argc, argv, "--threads="),
			[](const std::string & value) { return static_cast<unsigned int>(std::stoul(value)); },
			std::max(1U, std::thread::hardware_concurrency()));
		const std::uint64_t record_threshold = parse_option_or<std::uint64_t>(
			option_value(argc, argv, "--threshold="), [](const std::string & value) { return static_cast<std::uint64_t>(std::stoull(value)); }, 1);
		const auto record_output = option_value(argc, argv, "--record-output=");

		// Per-argument domains: start from the function defaults, then let the command line clip any axis
		// (--lo/--hi/--scale/--min-exp/--max-exp, optionally suffixed with the argument index). Any such
		// flag, or --confine, switches the corpus to the confined region builder for every arity.
		std::array<domain, N> domains;
		for (std::size_t axis = 0; axis < N; ++axis)
		{
			domains[axis] = parse_axis_domain(argc, argv, axis, default_domains[axis]);
		}
		const bool region_mode = confine || domain_flags_present(argc, argv, N);

		// Optional sink for the inputs whose ULP distance reaches --threshold (default 1), written to
		// the --record-output text file one case per line as they are found. Omit --record-output and
		// no record file is written, the same way omitting --log-output writes no summary JSON.
		std::unique_ptr<threshold_recorder> recorder;
		if (record_output.has_value())
		{
			recorder = std::make_unique<threshold_recorder>(*record_output, record_threshold);
		}
		threshold_recorder * recorder_ptr = recorder.get();

		const bool run_exhaustive = exhaustive_flag && N == 1 && std::is_same_v<T, float>;

		// Deterministic seed measured alongside the random draws in the default sampled sweep: the
		// function hard cases and boundary inputs, any --input-file cases, and their nextafter neighbours
		// when --probe-neighbors is set. A targeted region sweep skips the global seed but still honours
		// an explicit --input-file. The exhaustive sweep already covers these inputs and uses none.
		std::vector<std::array<T, N>> seeded;
		if (!region_mode)
		{
			seeded = std::move(structured_seed);
		}
		if (input_file.has_value())
		{
			auto file_cases = read_input_file<T, N>(*input_file);
			seeded.insert(seeded.end(), file_cases.begin(), file_cases.end());
		}
		if (probe_neighbors && !seeded.empty())
		{
			add_neighbor_probes<T, N>(seeded);
		}

		std::vector<bit_interval> intervals;
		std::string corpus = region_mode ? (confine ? "confined region" : "region") : "sampled";
		if (run_exhaustive)
		{
			intervals			  = float_exhaustive_intervals(argc, argv, domains[0]);
			const bool full_space = intervals.size() == 1 && intervals[0].begin == 0 && intervals[0].end == (std::uint64_t{ 1 } << 32);
			corpus				  = full_space ? "exhaustive binary32" : "exhaustive binary32 sub-range";
		}

		std::cout << "mpfr " << function_name << " impl=" << implementation << " type=" << scalar_type_name<T>() << " arity=" << N << " corpus=" << corpus
				  << " modes=" << rounding_modes.size() << " precision=" << precision << " threads=" << thread_count << '\n';
		if (run_exhaustive)
		{
			for (const bit_interval & interval : intervals)
			{
				std::cout << "  sweep [0x" << std::hex << interval.begin << ", 0x" << (interval.end - 1) << std::dec << "] " << (interval.end - interval.begin)
						  << " patterns\n";
			}
		} else if (region_mode)
		{
			for (std::size_t axis = 0; axis < N; ++axis)
			{
				std::cout << "  arg" << axis << " lo=" << domains[axis].lo << " hi=" << domains[axis].hi
						  << " scale=" << (domains[axis].log_magnitude ? "log" : "linear");
				if (domains[axis].log_magnitude)
				{
					std::cout << " exp=[" << domains[axis].min_exp << ", " << domains[axis].max_exp << "]";
				}
				std::cout << '\n';
			}
		}
		if (recorder_ptr != nullptr)
		{
			std::cout << "recording inputs with ulp >= " << record_threshold << " to " << *record_output << '\n';
		}
		if (!seeded.empty() && !run_exhaustive)
		{
			std::cout << "seeded cases: " << seeded.size() << (probe_neighbors ? " (with neighbour probes)" : "") << '\n';
		}

		const auto started = std::chrono::steady_clock::now();
		campaign_summary<T, N> summary;

		const auto sample = [&]() {
			auto cases = region_mode ? build_region_corpus<T, N>(domains, seed, count, confine) : build_corpus(seed, count);
			if (!seeded.empty())
			{
				cases.insert(cases.end(), seeded.begin(), seeded.end());
			}
			// A random sweep can draw the same input more than once and across the structured edges, so
			// drop exact-bit repeats unless --allow-duplicates is given. The exhaustive sweep is unique
			// already and never reaches here.
			if (!allow_duplicates)
			{
				const std::size_t removed = deduplicate_cases<T, N>(cases);
				if (removed != 0)
				{
					std::cout << "deduplicated: removed " << removed << " repeat input" << (removed == 1 ? "" : "s") << '\n';
				}
			}
			return run_sampled<T, N>(cases, rounding_modes, precision, max_ulp, thread_count, kernel, reference, std_reference, recorder_ptr, show_progress);
		};

		if constexpr (N == 1 && std::is_same_v<T, float>)
		{
			if (run_exhaustive)
			{
				summary = run_exhaustive_float(
					intervals, rounding_modes, precision, max_ulp, thread_count, kernel, reference, std_reference, recorder_ptr, show_progress);
			} else
			{
				summary = sample();
			}
		} else
		{
			summary = sample();
		}

		const auto elapsed		 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - started);
		const double average_ulp = summary.finite_count != 0 ? static_cast<double>(summary.ulp_sum / static_cast<long double>(summary.finite_count)) : 0.0;

		std::cout << "function=" << function_name << " impl=" << implementation << " type=" << scalar_type_name<T>() << " corpus=" << corpus
				  << " cases=" << summary.case_count << " max_ulp=" << summary.max_observed_ulp << " avg_ulp=" << average_ulp
				  << " above_target=" << summary.above_target_count << " hard_failures=" << summary.failure_count << " elapsed_ms=" << elapsed.count() << '\n';

		// Distribution of finite scored cases by ULP distance, the last bucket being everything past the
		// 4 ULP contract. It shows how close to correctly rounded the function is, not just its worst case.
		std::cout << "ulp_histogram finite=" << summary.finite_count;
		for (std::size_t i = 0; i < ulp_bucket_count; ++i)
		{
			std::cout << ' ' << (i + 1 == ulp_bucket_count ? std::to_string(i) + "+" : std::to_string(i)) << '=' << summary.ulp_histogram[i];
		}
		std::cout << '\n';

		if (output_path.has_value())
		{
			campaign_report<T, N> report;
			report.function_name	   = std::string(function_name);
			report.input_type		   = scalar_type_name<T>();
			report.implementation	   = std::string(implementation);
			report.oracle			   = "mpfr";
			report.corpus			   = corpus;
			report.rounding_modes	   = rounding_modes_label(rounding_modes);
			report.case_count		   = summary.case_count;
			report.hard_failure_count  = summary.failure_count;
			report.above_target_count  = summary.above_target_count;
			report.finite_count		   = summary.finite_count;
			report.max_observed_ulp	   = summary.max_observed_ulp;
			report.average_ulp		   = average_ulp;
			report.ulp_histogram	   = summary.ulp_histogram;
			report.worst_inputs_bits   = join_input_bits<T, N>(summary.worst_inputs);
			report.worst_actual_bits   = bits_hex(summary.worst_actual);
			report.worst_expected_bits = bits_hex(summary.worst_expected);
			report.oracle_precision	   = static_cast<unsigned long>(precision);
			report.compiler			   = compiler_id();
			report.platform			   = platform_id();
			report.optimization_mode   = optimization_mode();
			report.seed				   = seed;
			report.elapsed_ms		   = static_cast<std::uint64_t>(elapsed.count());
			report.timestamp		   = utc_timestamp();
			write_campaign_summary_json<T, N>(*output_path, report);
			std::cout << "summary: " << *output_path << '\n';
		}

		if (summary.failure_count != 0)
		{
			std::cout << "reference disagreements (inf/zero/overflow/ulp ceiling): " << summary.failure_count << '\n';
		}
		return summary.failure_count == 0 ? 0 : 1;
	}
} // namespace ccm::test::oracle::unary_oracle
