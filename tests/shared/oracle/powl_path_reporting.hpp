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

#include "ccmath/internal/config/long_double_format.hpp"
#include "ccmath/internal/config/powl_policy.hpp"
#include "ccmath/internal/math/generic/func/power/powl_gen.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/math/compare/isnan.hpp"

#include <cmath>
#include <cstdint>

namespace ccm::test::oracle
{
	enum class PowlImplementationPath
	{
		Ld64DoubleAlias,
		Ld80SpecialCase,
		Ld80BoundedInteger,
		Ld80GeneralFinite,
		Ld80ReducedPrecisionFallback,
		Ld80Unsupported,
		Ld128ReducedPrecisionFallback,
		Ld128Unsupported,
		UnknownReducedPrecisionFallback,
		UnknownUnsupported,
	};

	constexpr bool powl_path_is_reduced_precision(PowlImplementationPath path) noexcept
	{
		switch (path)
		{
		case PowlImplementationPath::Ld64DoubleAlias:
		case PowlImplementationPath::Ld80SpecialCase:
		case PowlImplementationPath::Ld80BoundedInteger:
		case PowlImplementationPath::Ld80GeneralFinite				: return false;
		case PowlImplementationPath::Ld80ReducedPrecisionFallback	:
		case PowlImplementationPath::Ld128ReducedPrecisionFallback	:
		case PowlImplementationPath::UnknownReducedPrecisionFallback: return true;
		case PowlImplementationPath::Ld80Unsupported				:
		case PowlImplementationPath::Ld128Unsupported				:
		case PowlImplementationPath::UnknownUnsupported				: return false;
		}
		return false;
	}

	constexpr bool powl_path_is_native_phase1(PowlImplementationPath path) noexcept
	{
		switch (path)
		{
		case PowlImplementationPath::Ld64DoubleAlias:
		case PowlImplementationPath::Ld80SpecialCase:
		case PowlImplementationPath::Ld80BoundedInteger:
		case PowlImplementationPath::Ld80GeneralFinite : return true;
		default										   : return false;
		}
	}

	constexpr const char * powl_path_name(PowlImplementationPath path) noexcept
	{
		switch (path)
		{
		case PowlImplementationPath::Ld64DoubleAlias				: return "ld64_double_alias";
		case PowlImplementationPath::Ld80SpecialCase				: return "ld80_special_case";
		case PowlImplementationPath::Ld80BoundedInteger				: return "ld80_bounded_integer";
		case PowlImplementationPath::Ld80GeneralFinite				: return "ld80_general_finite";
		case PowlImplementationPath::Ld80ReducedPrecisionFallback	: return "ld80_reduced_precision_fallback";
		case PowlImplementationPath::Ld80Unsupported				: return "ld80_unsupported";
		case PowlImplementationPath::Ld128ReducedPrecisionFallback	: return "ld128_reduced_precision_fallback";
		case PowlImplementationPath::Ld128Unsupported				: return "ld128_unsupported";
		case PowlImplementationPath::UnknownReducedPrecisionFallback: return "unknown_reduced_precision_fallback";
		case PowlImplementationPath::UnknownUnsupported				: return "unknown_unsupported";
		}
		return "unknown_unsupported";
	}

	namespace powl_path_detail
	{
		constexpr bool is_ld80_special_case_route(long double base, long double exp) noexcept
		{
			if (exp == 0.0L)
			{
				return true;
			}
			if (base == 1.0L)
			{
				return true;
			}
			if (ccm::isnan(base) || ccm::isnan(exp))
			{
				return true;
			}

			using FPBits_t = support::fp::FPBits<long double>;
			const FPBits_t base_bits(base);
			const FPBits_t exp_bits(exp);

			if (base_bits.is_zero())
			{
				return true;
			}
			if (base_bits.is_inf())
			{
				return true;
			}
			if (exp_bits.is_inf())
			{
				return true;
			}
			if (base_bits.is_neg() && !gen::internal::powl_bits::is_integer(exp))
			{
				return true;
			}
			return false;
		}

		constexpr bool is_ld80_bounded_integer_route([[maybe_unused]] long double base, long double exp) noexcept
		{
			if (is_ld80_special_case_route(base, exp))
			{
				return false;
			}

			std::int64_t ignored = 0;
			return gen::internal::powl_bits::try_extract_int64(exp, ignored);
		}

		constexpr PowlImplementationPath classify_ld80_path(long double base, long double exp) noexcept
		{
			if (is_ld80_special_case_route(base, exp))
			{
				return PowlImplementationPath::Ld80SpecialCase;
			}
			if (is_ld80_bounded_integer_route(base, exp))
			{
				return PowlImplementationPath::Ld80BoundedInteger;
			}
			return PowlImplementationPath::Ld80GeneralFinite;
		}

	} // namespace powl_path_detail

	constexpr PowlImplementationPath classify_powl_gen_path(long double base, long double exp) noexcept
	{
		switch (config::detect_long_double_format())
		{
		case config::LongDoubleFormat::Double	  : return PowlImplementationPath::Ld64DoubleAlias;
		case config::LongDoubleFormat::X87Extended: return powl_path_detail::classify_ld80_path(base, exp);
		case config::LongDoubleFormat::IEEEBinary128:
			return config::reduced_precision_powl_fallback_enabled() ? PowlImplementationPath::Ld128ReducedPrecisionFallback
																	 : PowlImplementationPath::Ld128Unsupported;
		case config::LongDoubleFormat::Unknown:
			return config::reduced_precision_powl_fallback_enabled() ? PowlImplementationPath::UnknownReducedPrecisionFallback
																	 : PowlImplementationPath::UnknownUnsupported;
		}
		return PowlImplementationPath::UnknownUnsupported;
	}

} // namespace ccm::test::oracle
