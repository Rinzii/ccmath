#pragma once

#include "ccmath/internal/config/type_support.hpp"

#include <cmath>
#include <limits>
#include <string>
#include <vector>

namespace ccm::test::oracle::cross_libm
{
	enum class disagreement_kind
	{
		ulp_ordinary,
		exceptional_policy,
		nan_sign,
		signed_zero,
		exact_match,
	};

	struct backend_info
	{
		std::string id;
		std::string label;
		bool available = false;
	};

	inline std::string platform_libm_label()
	{
#if defined(__APPLE__)
		return "apple_libsystem";
#elif defined(_WIN32)
		return "msvc_crt";
#elif defined(__linux__)
		return "glibc";
#else
		return "platform_libm";
#endif
	}

	inline std::vector<backend_info> detect_backends()
	{
		std::vector<backend_info> backends;
		backends.push_back({ "std_pow", "std::pow", true });
		backends.push_back({ "platform_libm", platform_libm_label(), true });
#ifdef CCMATH_CROSS_LIBM_OPENLIBM
		backends.push_back({ "openlibm", "OpenLibm", true });
#else
		backends.push_back({ "openlibm", "OpenLibm", false });
#endif
#ifdef CCMATH_CROSS_LIBM_SLEEF
		backends.push_back({ "sleef", "SLEEF", true });
#else
		backends.push_back({ "sleef", "SLEEF", false });
#endif
#ifdef CCMATH_CROSS_LIBM_LLVM_LIBC
		backends.push_back({ "llvm_libc", "LLVM libc", true });
#else
		backends.push_back({ "llvm_libc", "LLVM libc", false });
#endif
		return backends;
	}

	template <typename T>
	inline disagreement_kind classify(T lhs, T rhs)
	{
		if (lhs == rhs) { return disagreement_kind::exact_match; }
		if (std::isnan(lhs) && std::isnan(rhs)) { return disagreement_kind::exceptional_policy; }
		if (std::isinf(lhs) && std::isinf(rhs) && std::signbit(lhs) == std::signbit(rhs)) { return disagreement_kind::exact_match; }
		if (lhs == T{} && rhs == T{}) { return disagreement_kind::signed_zero; }
		return disagreement_kind::ulp_ordinary;
	}

	inline const char * disagreement_name(disagreement_kind kind)
	{
		switch (kind)
		{
		case disagreement_kind::ulp_ordinary: return "ulp_ordinary";
		case disagreement_kind::exceptional_policy: return "exceptional_policy";
		case disagreement_kind::nan_sign: return "nan_sign";
		case disagreement_kind::signed_zero: return "signed_zero";
		case disagreement_kind::exact_match: return "exact_match";
		}
		return "unknown";
	}
} // namespace ccm::test::oracle::cross_libm
