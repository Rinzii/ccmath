#include "pow_adversarial_modes.hpp"
#include "mpfr_pow_common.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace
{
	template <typename T, typename Fn>
	std::vector<ccm::test::oracle::failure_record<T>> run_search(std::string_view function_name,
																		  std::string_view path_name,
																		  ccm::test::pow_path::validation_path path,
																		  std::string_view search_mode,
																		  Fn fn,
																		  const std::vector<ccm::test::oracle::pow_case<T>> & cases,
																		  mpfr_prec_t precision,
																		  std::uint64_t max_ulp,
																		  std::uint64_t seed)
	{
		std::vector<ccm::test::oracle::failure_record<T>> failures;
		ccm::test::oracle::run_summary<T> summary;
		for (const auto & test_case : cases)
		{
			if (auto failure = ccm::test::oracle::evaluate_case(
					test_case, function_name, path_name, path, fn, precision, max_ulp, summary, seed, search_mode))
			{
				failures.push_back(*failure);
			}
		}
		std::sort(
			failures.begin(), failures.end(), [](const auto & lhs, const auto & rhs) { return lhs.ulp_distance > rhs.ulp_distance; });
		return failures;
	}

	ccm::test::pow_path::validation_path parse_search_path(int argc, char ** argv)
	{
		const std::string path = ccm::test::oracle::option_value(argc, argv, "--path=").value_or("public_default");
		if (auto parsed = ccm::test::pow_path::parse_path(path)) { return *parsed; }
		return ccm::test::pow_path::validation_path::public_default;
	}
} // namespace

int main(int argc, char ** argv)
{
	const std::string type = ccm::test::oracle::option_value(argc, argv, "--type=").value_or("double");
	const std::string mode = ccm::test::oracle::option_value(argc, argv, "--mode=").value_or("random-bit-pattern");
	const std::string output_path = ccm::test::oracle::option_value(argc, argv, "--json-output=").value_or("pow-search-results.json");
	const std::uint64_t seed =
		ccm::test::oracle::parse_option_or<std::uint64_t>(ccm::test::oracle::option_value(argc, argv, "--seed="),
			[](const std::string & value) { return std::stoull(value); },
			0xBAD5EEDULL);
	const std::size_t count =
		ccm::test::oracle::parse_option_or<std::size_t>(ccm::test::oracle::option_value(argc, argv, "--count="),
			[](const std::string & value) { return static_cast<std::size_t>(std::stoull(value)); },
			4096);
	const std::uint64_t max_ulp =
		ccm::test::oracle::parse_option_or<std::uint64_t>(ccm::test::oracle::option_value(argc, argv, "--max-ulp="),
			[](const std::string & value) { return std::stoull(value); },
			4);
	const mpfr_prec_t precision =
		ccm::test::oracle::parse_option_or<mpfr_prec_t>(ccm::test::oracle::option_value(argc, argv, "--oracle-precision="),
			[](const std::string & value) { return static_cast<mpfr_prec_t>(std::stoul(value)); },
			256);

	const auto path = parse_search_path(argc, argv);
	if (type == "float")
	{
		const auto support = ccm::test::pow_path::path_is_supported<float>(path);
		if (!support.supported)
		{
			std::cout << "SKIP path=" << ccm::test::pow_path::path_name(path) << " reason=" << support.skip_reason << '\n';
			return 0;
		}
	}
	else
	{
		const auto support = ccm::test::pow_path::path_is_supported<double>(path);
		if (!support.supported)
		{
			std::cout << "SKIP path=" << ccm::test::pow_path::path_name(path) << " reason=" << support.skip_reason << '\n';
			return 0;
		}
	}

	const std::string meta_path = output_path + ".meta.json";
	{
		std::ofstream meta(meta_path, std::ios::trunc);
		meta << "{\n";
		meta << "  \"search_mode\": \"" << mode << "\",\n";
		meta << "  \"seed\": " << seed << ",\n";
		meta << "  \"path\": \"" << ccm::test::pow_path::path_name(path) << "\",\n";
		meta << "  \"configuration_name\": \"" << ccm::test::oracle::configuration_name() << "\",\n";
		meta << "  \"compiler\": \"" << ccm::test::oracle::json_escape(ccm::test::oracle::compiler_id()) << "\",\n";
		meta << "  \"platform\": \"" << ccm::test::oracle::platform_id() << "\",\n";
		meta << "  \"optimization_mode\": \"" << ccm::test::oracle::optimization_mode() << "\",\n";
		meta << "  \"timestamp\": \"" << ccm::test::oracle::utc_timestamp() << "\"\n";
		meta << "}\n";
	}

	if (type == "float")
	{
		const auto cases = ccm::test::oracle::adversarial::build_search_cases<float>(mode, seed, count);
		const auto failures = run_search<float>(
			"ccm::powf",
			ccm::test::pow_path::path_name(path),
			path,
			mode,
			[ path ](float base, float exponent) { return ccm::test::pow_path::invoke(path, base, exponent); },
			cases,
			precision,
			max_ulp,
			seed);
		ccm::test::oracle::write_failure_json(output_path, failures);
		std::cout << "wrote " << failures.size() << " candidate failures to " << output_path << " meta=" << meta_path << '\n';
		return 0;
	}

	const auto cases = ccm::test::oracle::adversarial::build_search_cases<double>(mode, seed, count);
	const auto failures = run_search<double>(
		"ccm::pow",
		ccm::test::pow_path::path_name(path),
		path,
		mode,
		[ path ](double base, double exponent) { return ccm::test::pow_path::invoke(path, base, exponent); },
		cases,
		precision,
		max_ulp,
		seed);
	ccm::test::oracle::write_failure_json(output_path, failures);
	std::cout << "wrote " << failures.size() << " candidate failures to " << output_path << " meta=" << meta_path << '\n';
	return 0;
}
