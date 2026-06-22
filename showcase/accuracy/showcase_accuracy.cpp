#include "accuracy/unary_mpfr_runner.hpp"
#include "ccmath_paths.hpp"

#include <iostream>
#include <optional>
#include <string>
#include <string_view>

namespace
{
	std::optional<std::string> option_value(int argc, char ** argv, std::string_view prefix)
	{
		for (int i = 1; i < argc; ++i)
		{
			const std::string_view arg = argv[i];
			if (arg.rfind(prefix, 0) == 0) { return std::string(arg.substr(prefix.size())); }
		}
		return std::nullopt;
	}

	std::optional<ccm::showcase::accuracy::unary_function> parse_function(std::string_view raw)
	{
		if (raw == "sqrt") { return ccm::showcase::accuracy::unary_function::sqrt_fn; }
		if (raw == "sin") { return ccm::showcase::accuracy::unary_function::sin_fn; }
		return std::nullopt;
	}
} // namespace

int main(int argc, char ** argv)
{
	const auto fn_raw = option_value(argc, argv, "--function=");
	if (!fn_raw.has_value())
	{
		std::cerr << "usage: showcase_accuracy --function=sqrt|sin [--path=all|ccmath_path] [--backend=all|ccmath|gcem] [--log-output=path.json]\n";
		return 2;
	}

	const auto fn = parse_function(*fn_raw);
	if (!fn.has_value())
	{
		std::cerr << "unknown function: " << *fn_raw << '\n';
		return 2;
	}

	const auto path_raw			= option_value(argc, argv, "--path=").value_or("all");
	const auto backend_raw		= option_value(argc, argv, "--backend=").value_or("all");
	const auto log_output		= option_value(argc, argv, "--log-output=");
	const mpfr_prec_t precision = 256;
	const std::uint64_t max_ulp = 4;

	mpfr_set_default_rounding_mode(MPFR_RNDN);

	int exit_code = 0;

	const auto run_ccmath = [&](ccm::showcase::ccmath_path::path path)
	{
		std::optional<std::string> path_log;
		if (log_output.has_value()) { path_log = *log_output + "." + std::string(ccm::showcase::ccmath_path::name(path)) + ".json"; }
		exit_code |= ccm::showcase::accuracy::run_ccmath_path_campaign(*fn, path, precision, max_ulp, path_log);
	};

	if (backend_raw == "all" || backend_raw == "ccmath")
	{
		if (path_raw == "all")
		{
			run_ccmath(ccm::showcase::ccmath_path::path::public_default);
			run_ccmath(ccm::showcase::ccmath_path::path::generic_gen);
			run_ccmath(ccm::showcase::ccmath_path::path::runtime_rt);
			run_ccmath(ccm::showcase::ccmath_path::path::runtime_simd);
			run_ccmath(ccm::showcase::ccmath_path::path::runtime_builtin);
		}
		else if (const auto path = ccm::showcase::ccmath_path::parse(path_raw)) { run_ccmath(*path); }
		else
		{
			std::cerr << "unknown path: " << path_raw << '\n';
			return 2;
		}
	}

	if (backend_raw == "all" || backend_raw == "gcem")
	{
		std::optional<std::string> gcem_log;
		if (log_output.has_value()) { gcem_log = *log_output + ".gcem.json"; }
		exit_code |= ccm::showcase::accuracy::run_gcem_campaign(*fn, precision, max_ulp, gcem_log);
	}

	return exit_code;
}
