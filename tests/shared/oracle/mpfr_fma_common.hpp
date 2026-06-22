#pragma once

#include "ccmath/internal/math/generic/builtins/basic/fma.hpp"
#include "ccmath/internal/support/fp/fma.hpp"
#include "oracle_campaign_ternary.hpp"
#include "utils/math_samples.hpp"
#include "utils/test_runtime.hpp"

#include <array>
#include <chrono>
#include <cmath>
#include <limits>
#include <optional>
#include <random>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace ccm::test::oracle::fma_oracle
{
	enum class validation_path
	{
		public_default,
		software_fallback,
		quiet_generic,
	};

	inline std::string path_name(validation_path path)
	{
		switch (path)
		{
		case validation_path::public_default: return "public_default";
		case validation_path::software_fallback: return "software_fallback";
		case validation_path::quiet_generic: return "quiet_generic";
		}
		return "unknown";
	}

	inline std::optional<validation_path> parse_path(std::string_view raw)
	{
		if (raw == "public" || raw == "public_default") { return validation_path::public_default; }
		if (raw == "software" || raw == "software_fallback") { return validation_path::software_fallback; }
		if (raw == "quiet" || raw == "quiet_generic") { return validation_path::quiet_generic; }
		return std::nullopt;
	}

	inline std::vector<validation_path> parse_paths(int argc, char** argv)
	{
		std::vector<validation_path> paths;
		const auto raw = option_value(argc, argv, "--path=");
		if (raw.has_value())
		{
			std::string_view view = *raw;
			while (!view.empty())
			{
				const std::size_t comma		 = view.find(',');
				const std::string_view token = view.substr(0, comma);
				if (auto parsed = parse_path(token)) { paths.push_back(*parsed); }
				if (comma == std::string_view::npos) { break; }
				view.remove_prefix(comma + 1);
			}
		}
		if (paths.empty()) { paths.push_back(validation_path::public_default); }
		return paths;
	}

	template <typename T>
	inline T invoke(validation_path path, T x, T y, T z)
	{
		switch (path)
		{
		case validation_path::public_default:
			if constexpr (std::is_same_v<T, float>) { return ccm::fmaf(ccm::test::runtime_value(x), ccm::test::runtime_value(y), ccm::test::runtime_value(z)); }
			else
			{
				return ccm::fma(ccm::test::runtime_value(x), ccm::test::runtime_value(y), ccm::test::runtime_value(z));
			}
		case validation_path::software_fallback:
			return ccm::support::fp::public_fma(ccm::test::runtime_value(x), ccm::test::runtime_value(y), ccm::test::runtime_value(z));
		case validation_path::quiet_generic:
			return ccm::support::fp::generic_fma(ccm::test::runtime_value(x), ccm::test::runtime_value(y), ccm::test::runtime_value(z));
		}
		return ccm::support::fp::public_fma(ccm::test::runtime_value(x), ccm::test::runtime_value(y), ccm::test::runtime_value(z));
	}

	template <typename T>
	inline T std_fma_reference(T x, T y, T z)
	{
		if constexpr (std::is_same_v<T, float>) { return std::fmaf(x, y, z); }
		else
		{
			return std::fma(x, y, z);
		}
	}

	template <typename T>
	inline T mpfr_fma_reference(T x, T y, T z, mpfr_prec_t precision, mpfr_rnd_t rounding)
	{
		mpfr_t x_mpfr;
		mpfr_t y_mpfr;
		mpfr_t z_mpfr;
		mpfr_t result_mpfr;
		mpfr_init2(x_mpfr, precision);
		mpfr_init2(y_mpfr, precision);
		mpfr_init2(z_mpfr, precision);
		mpfr_init2(result_mpfr, precision);

		mpfr_set_scalar(x_mpfr, x);
		mpfr_set_scalar(y_mpfr, y);
		mpfr_set_scalar(z_mpfr, z);
		mpfr_fma(result_mpfr, x_mpfr, y_mpfr, z_mpfr, rounding);

		const T result = mpfr_get_scalar<T>(result_mpfr, rounding);
		mpfr_clear(x_mpfr);
		mpfr_clear(y_mpfr);
		mpfr_clear(z_mpfr);
		mpfr_clear(result_mpfr);
		return result;
	}

	template <typename T>
	inline void append_structured_cases(std::vector<ternary_case<T>>& cases)
	{
		if constexpr (std::is_same_v<T, float>)
		{
			for (const auto& input : ccm::test::samples::kFmaFloatCases) { cases.push_back({ input.x, input.y, input.z, "structured regression corpus" }); }
		}
		else
		{
			for (const auto& input : ccm::test::samples::kFmaDoubleCases) { cases.push_back({ input.x, input.y, input.z, "structured regression corpus" }); }
		}

		const T one		  = static_cast<T>(1);
		const T next_up	  = std::nextafter(one, std::numeric_limits<T>::infinity());
		const T next_down = std::nextafter(one, static_cast<T>(0));
		cases.push_back({ next_up, next_up, -one, "near-unity cancellation above 1" });
		cases.push_back({ next_down, next_down, -one, "near-unity cancellation below 1" });
		cases.push_back({ next_down, next_up, -one, "mixed-neighbor near-unity cancellation" });

		const T min_normal	  = std::numeric_limits<T>::min();
		const T min_subnormal = std::numeric_limits<T>::denorm_min();
		if constexpr (std::is_same_v<T, float>)
		{
			cases.push_back({ min_normal, 0x1.0p-23F, -min_subnormal, "float exact subnormal cancellation" });
			cases.push_back({ min_normal, 0x1.0p-24F, -min_subnormal, "float sticky-bit underflow boundary" });
		}
		else
		{
			cases.push_back({ min_normal, 0x1.0p-52, -min_subnormal, "double exact subnormal cancellation" });
			cases.push_back({ min_normal, 0x1.0p-53, -min_subnormal, "double sticky-bit underflow boundary" });
		}
	}

	template <typename T>
	inline void append_special_matrix(std::vector<ternary_case<T>>& cases)
	{
		const std::array<T, 10> values = {
			-std::numeric_limits<T>::infinity(),
			-std::numeric_limits<T>::max(),
			-static_cast<T>(1),
			-static_cast<T>(0),
			static_cast<T>(0),
			std::numeric_limits<T>::denorm_min(),
			std::numeric_limits<T>::min(),
			static_cast<T>(1),
			std::numeric_limits<T>::max(),
			std::numeric_limits<T>::infinity(),
		};

		for (T x : values)
		{
			for (T y : values)
			{
				for (T z : values) { cases.push_back({ x, y, z, "special-value matrix" }); }
			}
		}

		cases.push_back({ std::numeric_limits<T>::quiet_NaN(), static_cast<T>(1), static_cast<T>(1), "quiet NaN multiplicand" });
		cases.push_back({ static_cast<T>(1), std::numeric_limits<T>::quiet_NaN(), static_cast<T>(1), "quiet NaN multiplier" });
		cases.push_back({ static_cast<T>(1), static_cast<T>(1), std::numeric_limits<T>::quiet_NaN(), "quiet NaN addend" });
	}

	template <typename T>
	inline void append_random_cases(std::vector<ternary_case<T>>& cases, campaign_mode mode, std::uint64_t seed)
	{
		const std::size_t count = mode == campaign_mode::quick ? (std::is_same_v<T, float> ? 1024U : 512U)
															   : (mode == campaign_mode::extended ? (std::is_same_v<T, float> ? 8192U : 4096U)
																								  : (std::is_same_v<T, float> ? 32768U : 16384U));
		std::mt19937_64 rng(seed);
		for (std::size_t i = 0; i < count; ++i)
		{
			if constexpr (std::is_same_v<T, float>)
			{
				cases.push_back({
					ccm::support::bit_cast<float>(static_cast<std::uint32_t>(rng())),
					ccm::support::bit_cast<float>(static_cast<std::uint32_t>(rng())),
					ccm::support::bit_cast<float>(static_cast<std::uint32_t>(rng())),
					"deterministic random bit-pattern campaign",
				});
			}
			else
			{
				cases.push_back({
					ccm::support::bit_cast<double>(rng()),
					ccm::support::bit_cast<double>(rng()),
					ccm::support::bit_cast<double>(rng()),
					"deterministic random bit-pattern campaign",
				});
			}
		}
	}

	template <typename T>
	inline std::vector<ternary_case<T>> build_cases(campaign_mode mode, std::uint64_t seed)
	{
		std::vector<ternary_case<T>> cases;
		append_structured_cases(cases);
		append_special_matrix(cases);
		append_random_cases(cases, mode, seed);
		return cases;
	}

	template <typename T>
	inline int run_campaign(int argc, char** argv, std::string_view function_name, mpfr_prec_t default_precision)
	{
		const auto mode		   = parse_mode(option_value(argc, argv, "--mode="));
		const auto output_path = resolve_event_log_path(argc, argv);
		const std::uint64_t seed =
			parse_option_or<std::uint64_t>(option_value(argc, argv, "--seed="), [](const std::string& value) { return std::stoull(value); }, 0xF6A600D5EEDULL);
		const std::uint64_t max_ulp = parse_option_or<std::uint64_t>(
			option_value(argc, argv, "--max-ulp="), [](const std::string& value) { return static_cast<std::uint64_t>(std::stoull(value)); }, 0);
		const mpfr_prec_t precision = parse_option_or<mpfr_prec_t>(
			option_value(argc, argv, "--oracle-precision="),
			[](const std::string& value) { return static_cast<mpfr_prec_t>(std::stoul(value)); },
			default_precision);

		const auto paths		  = parse_paths(argc, argv);
		const auto rounding_modes = parse_rounding_modes(argc, argv);
		auto cases				  = build_cases<T>(mode, seed);
		std::vector<ternary_failure_record<T>> events;

		std::cout << "mpfr " << function_name << " campaign mode=" << mode_name(mode) << " fma=" << fma_status()
				  << " configuration=" << fma_configuration_name() << " rounding_modes=" << rounding_modes.size() << " cases=" << cases.size() << '\n';

		for (validation_path path : paths)
		{
			ternary_run_summary<T> summary;
			const auto started = std::chrono::steady_clock::now();

			for (const int rounding_mode : rounding_modes)
			{
				ScopedMpfrRoundingMode scope(rounding_mode);
				if (!scope) { continue; }
				for (const auto& test_case : cases)
				{
					if (auto failure = evaluate_ternary_mpfr_case(
							test_case,
							function_name,
							path_name(path),
							[path](T x, T y, T z) { return invoke(path, x, y, z); },
							[](T x, T y, T z, mpfr_prec_t oracle_precision, mpfr_rnd_t rounding)
							{ return mpfr_fma_reference(x, y, z, oracle_precision, rounding); },
							[](T x, T y, T z) { return std_fma_reference(x, y, z); },
							precision,
							max_ulp,
							seed,
							summary,
							"exceptional mismatch vs std::fma fallback"))
					{
						events.push_back(*failure);
					}
				}
			}

			const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - started);
			std::cout << "path=" << path_name(path) << " configuration=" << fma_configuration_name() << " rounding_modes=" << rounding_modes.size()
					  << " cases=" << summary.case_count << " mpfr_policy_mismatches=" << summary.mpfr_policy_mismatch_count
					  << " max_ulp=" << summary.max_observed_ulp << " failures=" << summary.failure_count << " elapsed_ms=" << elapsed.count() << '\n';
		}

		print_ternary_hard_failures(events);
		if (output_path.has_value())
		{
			write_ternary_failure_json(*output_path, events);
			std::cout << "oracle event log: " << *output_path << " events=" << events.size()
					  << " hard_failures=" << (has_ternary_hard_failure(events) ? "yes" : "no") << '\n';
		}

		return has_ternary_hard_failure(events) ? 1 : 0;
	}
} // namespace ccm::test::oracle::fma_oracle
