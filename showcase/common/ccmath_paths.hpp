#pragma once

#include "ccmath/ccmath.hpp"
#include "ccmath/internal/math/generic/func/power/sqrt_gen.hpp"
#include "ccmath/internal/math/generic/func/trig/sin_gen.hpp"
#include "ccmath/internal/math/runtime/func/power/sqrt_rt.hpp"
#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"
#include "ccmath/internal/math/runtime/func/trig/sin_rt.hpp"
#include "ccmath/internal/math/runtime/simd/func/catalog.hpp"
#include "ccmath/internal/predef/has_builtin.hpp"
#include "utils/test_runtime.hpp"

#include <cmath>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

namespace ccm::showcase::ccmath_path
{
	enum class path
	{
		public_default,
		generic_gen,
		runtime_rt,
		runtime_simd,
		runtime_builtin,
	};

	constexpr std::string_view name(path value) noexcept
	{
		switch (value)
		{
		case path::public_default: return "public_default";
		case path::generic_gen: return "generic_gen";
		case path::runtime_rt: return "runtime_rt";
		case path::runtime_simd: return "runtime_simd";
		case path::runtime_builtin: return "runtime_builtin";
		}
		return "unknown";
	}

	constexpr std::optional<path> parse(std::string_view raw) noexcept
	{
		if (raw == "public_default" || raw == "public") { return path::public_default; }
		if (raw == "generic_gen" || raw == "generic_runtime" || raw == "generic") { return path::generic_gen; }
		if (raw == "runtime_rt" || raw == "rt") { return path::runtime_rt; }
		if (raw == "runtime_simd" || raw == "simd") { return path::runtime_simd; }
		if (raw == "runtime_builtin" || raw == "builtin") { return path::runtime_builtin; }
		return std::nullopt;
	}

	struct path_support
	{
		bool supported = false;
		std::string skip_reason;
	};

	inline path_support sqrt_is_supported(path value)
	{
		switch (value)
		{
		case path::public_default:
		case path::generic_gen:
		case path::runtime_rt: return { true, {} };
		case path::runtime_simd:
#if defined(CCMATH_HAS_SIMD)
			return { true, {} };
#else
			return { false, "runtime_simd requires CCMATH_HAS_SIMD" };
#endif
		case path::runtime_builtin:
#if CCM_HAS_BUILTIN(__builtin_sqrt) || defined(__builtin_sqrt)
			return { true, {} };
#else
			return { false, "runtime_builtin requires __builtin_sqrt" };
#endif
		}
		return { false, "unknown path" };
	}

	inline path_support sin_is_supported(path value)
	{
		switch (value)
		{
		case path::public_default:
		case path::generic_gen:
		case path::runtime_rt: return { true, {} };
		case path::runtime_simd:
#if defined(CCMATH_HAS_SIMD) && !(CCM_HAS_BUILTIN(__builtin_sin) || defined(__builtin_sin))
			return { true, {} };
#else
			return { false, "runtime_simd sin requires SIMD without a trusted sin builtin" };
#endif
		case path::runtime_builtin:
#if CCM_HAS_BUILTIN(__builtin_sin) || defined(__builtin_sin)
			return { true, {} };
#else
			return { false, "runtime_builtin requires __builtin_sin" };
#endif
		}
		return { false, "unknown path" };
	}

	inline double invoke_sqrt(path value, double x)
	{
		const double input = ccm::test::runtime_value(x);
		switch (value)
		{
		case path::public_default: return ccm::sqrt(input);
		case path::generic_gen: return ccm::gen::sqrt_gen<double>(input);
		case path::runtime_rt: return ccm::rt::sqrt_rt<double>(input);
		case path::runtime_simd:
#if defined(CCMATH_HAS_SIMD)
			return ccm::rt::simd_impl::sqrt_simd_impl(input);
#else
			return ccm::gen::sqrt_gen<double>(input);
#endif
		case path::runtime_builtin:
#if CCM_HAS_BUILTIN(__builtin_sqrt) || defined(__builtin_sqrt)
			return __builtin_sqrt(input);
#else
			return ccm::gen::sqrt_gen<double>(input);
#endif
		}
		return ccm::sqrt(input);
	}

	inline double invoke_sin(path value, double x)
	{
		const double input = ccm::test::runtime_value(x);
		switch (value)
		{
		case path::public_default: return ccm::sin(input);
		case path::generic_gen: return ccm::gen::sin_gen(input);
		case path::runtime_rt: return ccm::rt::sin_rt(input);
		case path::runtime_simd:
#if defined(CCMATH_HAS_SIMD) && !(CCM_HAS_BUILTIN(__builtin_sin) || defined(__builtin_sin))
			return ccm::rt::simd_impl::unary_via_scalar_abi(input, [](double v) { return ccm::gen::sin_gen(v); });
#else
			return ccm::gen::sin_gen(input);
#endif
		case path::runtime_builtin:
#if CCM_HAS_BUILTIN(__builtin_sin) || defined(__builtin_sin)
			return __builtin_sin(input);
#else
			return ccm::gen::sin_gen(input);
#endif
		}
		return ccm::sin(input);
	}
} // namespace ccm::showcase::ccmath_path
