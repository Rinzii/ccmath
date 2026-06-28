#pragma once

#include "ccmath/ccmath.hpp"
#include "ccmath/internal/support/bits.hpp"

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

namespace ccm::test
{
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

	inline std::string optimization_mode()
	{
#if defined(NDEBUG)
		return "release";
#else
		return "debug";
#endif
	}

	inline constexpr const char * fma_status()
	{
#if defined(CCMATH_TARGET_CPU_HAS_FMA)
		return "on";
#else
		return "off";
#endif
	}

	inline std::string pow_configuration_name()
	{
#ifdef CCM_CONFIG_TEST_POW_VALIDATION_NAME
		return CCM_CONFIG_TEST_POW_VALIDATION_NAME;
#else
		return "default";
#endif
	}

	inline std::string fma_configuration_name()
	{
#ifdef CCM_CONFIG_DISABLE_RUNTIME_BUILTIN_FMA
		return "runtime_builtin_disabled";
#else
		return "default";
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

	template <typename T> inline constexpr const char * scalar_type_name()
	{
		if constexpr (std::is_same_v<T, float>)
		{
			return "float";
		} else if constexpr (std::is_same_v<T, double>)
		{
			return "double";
		} else if constexpr (std::is_same_v<T, long double>)
		{
			return "long double";
		} else
		{
			return "unknown";
		}
	}

	template <typename T> inline constexpr const char * pow_builtin_status()
	{
		return ccm::builtin::has_runtime_pow<T> ? "available" : "unavailable";
	}

	template <typename T> inline constexpr const char * fma_builtin_status()
	{
		if constexpr (ccm::builtin::runtime_builtin_fma_trusted<T>)
		{
			return "trusted";
		}
		if constexpr (ccm::builtin::has_fma<T>)
		{
			return "available";
		}
		return "unavailable";
	}

	inline constexpr const char * simd_status()
	{
#if defined(CCMATH_HAS_SIMD)
		return "compiled";
#else
		return "unavailable";
#endif
	}

	template <typename T> inline std::string bits_hex(T value)
	{
		using fp_bits	   = ccm::support::fp::FPBits<T>;
		using storage_type = typename fp_bits::storage_type;

		const storage_type bits = fp_bits(value).uintval();
		std::ostringstream oss;
		oss << "0x" << std::hex << std::setfill('0');
		if constexpr (sizeof(storage_type) <= sizeof(std::uint64_t))
		{
			oss << std::setw(static_cast<int>(sizeof(storage_type) * 2)) << static_cast<std::uint64_t>(bits);
		} else
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
			case '"' : oss << "\\\""; break;
			case '\n': oss << "\\n"; break;
			default	 : oss << ch; break;
			}
		}
		return oss.str();
	}
} // namespace ccm::test
