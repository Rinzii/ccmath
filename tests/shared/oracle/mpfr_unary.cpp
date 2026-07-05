/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "ccmath/ccmath.hpp"
#include "mpfr_unary_common.hpp"
#include "utils/test_runtime.hpp"

#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>

// Measurement executable for the elementary functions, generic over arity. Adding a function is one
// case below: name it, point it at the ccm kernel, the MPFR reference, the libm exceptional
// reference, and a corpus. Invoke as:
//   mpfr_unary --function=exp --type=float --exhaustive --rounding-modes=nearest
//   mpfr_unary --function=pow --type=float --mode=extended --count=5000000 --rounding-modes=nearest
//
// --impl picks which implementation is measured: gen (the generic kernel, the default), public (the
// ccm:: public path, which lowers to libm under FE_TONEAREST so it matches libm in round-to-nearest),
// or libm (the system math library). The implementation is recorded in the output and the summary JSON.
//   mpfr_unary --function=exp --type=double --impl=libm --rounding-modes=upward
//
// Region targeting. Clip any argument and the corpus switches to a confined region sample. The bounds
// are --lo/--hi (value range) and --scale=linear|log with --min-exp/--max-exp (sample across binades).
// An unsuffixed flag applies to every argument, a suffix picks one argument (--lo1 is pow's exponent):
//   mpfr_unary --function=exp --type=float --lo=0 --hi=1 --count=20000000
//   mpfr_unary --function=pow --type=float --lo0=1 --hi0=2 --scale1=log --min-exp1=-10 --max-exp1=10
// Add --confine to drop the structured edges and sample the region only. Add --progress for a live
// percent/throughput/ETA line on the long runs. A sampled sweep drops exact-bit repeat inputs by
// default so no value is measured twice; pass --allow-duplicates to keep them, which skips the dedup
// sort on a very large sample over a vast space (pow) where repeats are statistically absent anyway.
//
// Exhaustive binary32 can sweep a sub-range instead of all 2^32 patterns: pass a finite --lo/--hi
// (every float in that range is swept) or an explicit --exhaustive-range=START:END over bit patterns
// (decimal or 0x hex, inclusive):
//   mpfr_unary --function=sin --type=float --exhaustive --lo=1 --hi=2
//   mpfr_unary --function=cbrt --type=float --exhaustive-range=0x3f800000:0x40000000
//
// Every default sampled sweep also measures a deterministic seed of hard cases: the special-value
// matrix (pow corners such as pow(x, +/-0) and pow(negative, non-integer)), per-family branch points
// and cancellation regions (powers of two, the neighbourhood of 1, large multiples of pi/2, the gamma
// poles), and the exact over/underflow boundary inputs found by bisecting the reference. Replay an
// external worst-case table or a prior --record-output dump with --input-file (one case per line, N
// whitespace-separated values), and add --probe-neighbors to also test each seed's nextafter neighbours:
//   mpfr_unary --function=sin --type=double --input-file=worst_cases.txt --probe-neighbors
//
// Pass --record-output=<file> with --threshold=N to dump every input whose ULP distance is N or
// greater to a text file, one case per line, as they are found:
//   mpfr_unary --function=tan --type=float --exhaustive --threshold=2 --record-output=tan_above2.txt
namespace
{
	namespace oracle = ccm::test::oracle::unary_oracle;
	using oracle::domain;

	constexpr double kInf = std::numeric_limits<double>::infinity();

	// Convenience wrapper for the standard unary functions: a scalar ccm kernel, a standard MPFR
	// unary op, a scalar libm reference, and a sampling domain. It adapts the scalar callables to the
	// arity-1 array interface the generic campaign expects.
	// Convenience wrapper for the standard unary functions. --impl picks which of the three callables is
	// measured: the generic kernel (default), the public ccmath path, or the system libm. The MPFR
	// reference and the exceptional libm cross-check are unaffected by the choice.
	template <typename T, typename GenFn, typename PublicFn, typename StdFn>
	int run_standard(int argc,
					 char ** argv,
					 std::string_view name,
					 GenFn gen_scalar,
					 PublicFn public_scalar,
					 oracle::mpfr_unary_op op,
					 StdFn std_scalar,
					 domain dom,
					 oracle::function_family family)
	{
		const oracle::impl_kind impl = oracle::parse_impl(argc, argv);
		return oracle::run_campaign<T, 1>(
			argc,
			argv,
			name,
			[gen_scalar, public_scalar, std_scalar, impl](const std::array<T, 1> & inputs) -> T {
				switch (impl)
				{
				case oracle::impl_kind::libm	  : return std_scalar(inputs[0]);
				case oracle::impl_kind::public_api: return public_scalar(inputs[0]);
				case oracle::impl_kind::generic	  : break;
				}
				return gen_scalar(inputs[0]);
			},
			oracle::make_unary_ref<T>(op),
			[std_scalar](const std::array<T, 1> & inputs) { return std_scalar(inputs[0]); },
			[dom](std::uint64_t seed, std::size_t count) { return oracle::build_unary_corpus<T>(dom, seed, count); },
			std::array<domain, 1>{ dom },
			oracle::unary_structured_seed<T>(std_scalar, family),
			oracle::impl_token(impl));
	}

	template <typename T> int dispatch(std::string_view function, int argc, char ** argv)
	{
		if (function == "exp")
		{
			return run_standard<T>(
				argc,
				argv,
				"ccm::exp",
				[](T x) { return ccm::gen::exp_gen(ccm::test::runtime_value(x)); },
				[](T x) { return ccm::exp(ccm::test::runtime_value(x)); },
				mpfr_exp,
				[](T x) { return std::exp(x); },
				domain{ -745.0, 710.0 },
				oracle::function_family::exp);
		}
		if (function == "exp2")
		{
			return run_standard<T>(
				argc,
				argv,
				"ccm::exp2",
				[](T x) { return ccm::gen::exp2_gen(ccm::test::runtime_value(x)); },
				[](T x) { return ccm::exp2(ccm::test::runtime_value(x)); },
				mpfr_exp2,
				[](T x) { return std::exp2(x); },
				domain{ -1100.0, 1030.0 },
				oracle::function_family::exp);
		}
		if (function == "expm1")
		{
			return run_standard<T>(
				argc,
				argv,
				"ccm::expm1",
				[](T x) { return ccm::gen::expm1_gen(ccm::test::runtime_value(x)); },
				[](T x) { return ccm::expm1(ccm::test::runtime_value(x)); },
				mpfr_expm1,
				[](T x) { return std::expm1(x); },
				domain{ -50.0, 710.0 },
				oracle::function_family::exp);
		}
		if (function == "log")
		{
			return run_standard<T>(
				argc,
				argv,
				"ccm::log",
				[](T x) { return ccm::gen::log_gen(ccm::test::runtime_value(x)); },
				[](T x) { return ccm::log(ccm::test::runtime_value(x)); },
				mpfr_log,
				[](T x) { return std::log(x); },
				domain{ 0.0, kInf, true },
				oracle::function_family::logarithm);
		}
		if (function == "log1p")
		{
			return run_standard<T>(
				argc,
				argv,
				"ccm::log1p",
				[](T x) { return ccm::gen::log1p_gen(ccm::test::runtime_value(x)); },
				[](T x) { return ccm::log1p(ccm::test::runtime_value(x)); },
				mpfr_log1p,
				[](T x) { return std::log1p(x); },
				domain{ -1.0, kInf, true },
				oracle::function_family::logarithm);
		}
		if (function == "log2")
		{
			return run_standard<T>(
				argc,
				argv,
				"ccm::log2",
				[](T x) { return ccm::gen::log2_gen(ccm::test::runtime_value(x)); },
				[](T x) { return ccm::log2(ccm::test::runtime_value(x)); },
				mpfr_log2,
				[](T x) { return std::log2(x); },
				domain{ 0.0, kInf, true },
				oracle::function_family::logarithm);
		}
		if (function == "log10")
		{
			return run_standard<T>(
				argc,
				argv,
				"ccm::log10",
				[](T x) { return ccm::gen::log10_gen(ccm::test::runtime_value(x)); },
				[](T x) { return ccm::log10(ccm::test::runtime_value(x)); },
				mpfr_log10,
				[](T x) { return std::log10(x); },
				domain{ 0.0, kInf, true },
				oracle::function_family::logarithm);
		}
		if (function == "sin")
		{
			return run_standard<T>(
				argc,
				argv,
				"ccm::sin",
				[](T x) { return ccm::gen::sin_gen(ccm::test::runtime_value(x)); },
				[](T x) { return ccm::sin(ccm::test::runtime_value(x)); },
				mpfr_sin,
				[](T x) { return std::sin(x); },
				domain{ -kInf, kInf, true },
				oracle::function_family::trig);
		}
		if (function == "cos")
		{
			return run_standard<T>(
				argc,
				argv,
				"ccm::cos",
				[](T x) { return ccm::gen::cos_gen(ccm::test::runtime_value(x)); },
				[](T x) { return ccm::cos(ccm::test::runtime_value(x)); },
				mpfr_cos,
				[](T x) { return std::cos(x); },
				domain{ -kInf, kInf, true },
				oracle::function_family::trig);
		}
		if (function == "tan")
		{
			return run_standard<T>(
				argc,
				argv,
				"ccm::tan",
				[](T x) { return ccm::gen::tan_gen(ccm::test::runtime_value(x)); },
				[](T x) { return ccm::tan(ccm::test::runtime_value(x)); },
				mpfr_tan,
				[](T x) { return std::tan(x); },
				domain{ -kInf, kInf, true },
				oracle::function_family::trig);
		}
		if (function == "asin")
		{
			return run_standard<T>(
				argc,
				argv,
				"ccm::asin",
				[](T x) -> T {
					if constexpr (std::is_same_v<T, float>)
					{
						return ccm::internal::impl::asin_float(ccm::test::runtime_value(x));
					} else
					{
						return ccm::internal::impl::asin_double(ccm::test::runtime_value(x));
					}
				},
				[](T x) { return ccm::asin(ccm::test::runtime_value(x)); },
				mpfr_asin,
				[](T x) { return std::asin(x); },
				domain{ -1.0, 1.0 },
				oracle::function_family::inverse_trig);
		}
		if (function == "acos")
		{
			return run_standard<T>(
				argc,
				argv,
				"ccm::acos",
				[](T x) -> T {
					if constexpr (std::is_same_v<T, float>)
					{
						return ccm::internal::impl::acos_float(ccm::test::runtime_value(x));
					} else
					{
						return ccm::internal::impl::acos_double(ccm::test::runtime_value(x));
					}
				},
				[](T x) { return ccm::acos(ccm::test::runtime_value(x)); },
				mpfr_acos,
				[](T x) { return std::acos(x); },
				domain{ -1.0, 1.0 },
				oracle::function_family::inverse_trig);
		}
		if (function == "atan")
		{
			return run_standard<T>(
				argc,
				argv,
				"ccm::atan",
				[](T x) -> T {
					if constexpr (std::is_same_v<T, float>)
					{
						return ccm::internal::impl::atan_float(ccm::test::runtime_value(x));
					} else
					{
						return ccm::internal::impl::atan_double(ccm::test::runtime_value(x));
					}
				},
				[](T x) { return ccm::atan(ccm::test::runtime_value(x)); },
				mpfr_atan,
				[](T x) { return std::atan(x); },
				domain{ -kInf, kInf, true },
				oracle::function_family::inverse_trig);
		}
		if (function == "cbrt")
		{
			return run_standard<T>(
				argc,
				argv,
				"ccm::cbrt",
				[](T x) { return ccm::gen::cbrt_gen(ccm::test::runtime_value(x)); },
				[](T x) { return ccm::cbrt(ccm::test::runtime_value(x)); },
				mpfr_cbrt,
				[](T x) { return std::cbrt(x); },
				domain{ -kInf, kInf, true },
				oracle::function_family::power);
		}
		if (function == "sqrt")
		{
			return run_standard<T>(
				argc,
				argv,
				"ccm::sqrt",
				[](T x) { return ccm::gen::sqrt_gen(ccm::test::runtime_value(x)); },
				[](T x) { return ccm::sqrt(ccm::test::runtime_value(x)); },
				mpfr_sqrt,
				[](T x) { return std::sqrt(x); },
				domain{ 0.0, kInf, true },
				oracle::function_family::power);
		}
		if (function == "tgamma")
		{
			return run_standard<T>(
				argc,
				argv,
				"ccm::tgamma",
				[](T x) -> T {
					if constexpr (std::is_same_v<T, float>)
					{
						return ccm::internal::gamma_float(ccm::test::runtime_value(x));
					} else
					{
						return ccm::internal::gamma_double(ccm::test::runtime_value(x));
					}
				},
				[](T x) { return ccm::tgamma(ccm::test::runtime_value(x)); },
				mpfr_gamma,
				[](T x) { return std::tgamma(x); },
				domain{ -20.0, 171.0 },
				oracle::function_family::gamma);
		}
		if (function == "lgamma")
		{
			const oracle::impl_kind impl = oracle::parse_impl(argc, argv);
			return oracle::run_campaign<T, 1>(
				argc,
				argv,
				"ccm::lgamma",
				[impl](const std::array<T, 1> & inputs) -> T {
					switch (impl)
					{
					case oracle::impl_kind::libm	  : return std::lgamma(inputs[0]);
					case oracle::impl_kind::public_api: return ccm::lgamma(ccm::test::runtime_value(inputs[0]));
					case oracle::impl_kind::generic	  : break;
					}
					if constexpr (std::is_same_v<T, float>)
					{
						return ccm::internal::lgamma_float(ccm::test::runtime_value(inputs[0]));
					} else
					{
						return ccm::internal::lgamma_double(ccm::test::runtime_value(inputs[0]));
					}
				},
				oracle::make_lgamma_ref<T>(),
				[](const std::array<T, 1> & inputs) { return std::lgamma(inputs[0]); },
				[](std::uint64_t seed, std::size_t count) { return oracle::build_unary_corpus<T>(domain{ -50.0, kInf, true }, seed, count); },
				std::array<domain, 1>{ domain{ -50.0, kInf, true } },
				oracle::unary_structured_seed<T>([](T x) { return std::lgamma(x); }, oracle::function_family::gamma),
				oracle::impl_token(impl));
		}
		if (function == "pow")
		{
			const oracle::impl_kind impl = oracle::parse_impl(argc, argv);
			return oracle::run_campaign<T, 2>(
				argc,
				argv,
				"ccm::pow",
				[impl](const std::array<T, 2> & inputs) -> T {
					switch (impl)
					{
					case oracle::impl_kind::libm	  : return std::pow(inputs[0], inputs[1]);
					case oracle::impl_kind::public_api: return ccm::pow(ccm::test::runtime_value(inputs[0]), ccm::test::runtime_value(inputs[1]));
					case oracle::impl_kind::generic	  : break;
					}
					return ccm::gen::pow_gen(ccm::test::runtime_value(inputs[0]), ccm::test::runtime_value(inputs[1]));
				},
				oracle::make_pow_ref<T>(),
				[](const std::array<T, 2> & inputs) { return std::pow(inputs[0], inputs[1]); },
				[](std::uint64_t seed, std::size_t count) { return oracle::build_pow_corpus<T>(seed, count); },
				std::array<domain, 2>{ domain{ -kInf, kInf, true }, domain{ -kInf, kInf, true } },
				oracle::pow_special_matrix<T>(),
				oracle::impl_token(impl));
		}

		std::cerr << "unknown --function=" << function << '\n';
		return 2;
	}
} // namespace

int main(int argc, char ** argv)
{
	std::string function = "exp";
	std::string type	 = "double";
	for (int i = 1; i < argc; ++i)
	{
		const std::string_view arg(argv[i]);
		if (arg.rfind("--function=", 0) == 0)
		{
			function = std::string(arg.substr(std::string_view("--function=").size()));
		} else if (arg.rfind("--type=", 0) == 0)
		{
			type = std::string(arg.substr(std::string_view("--type=").size()));
		}
	}

	if (type == "float")
	{
		return dispatch<float>(function, argc, argv);
	}
	if (type == "double")
	{
		return dispatch<double>(function, argc, argv);
	}

	std::cerr << "unknown --type=" << type << " (expected float or double)\n";
	return 2;
}
